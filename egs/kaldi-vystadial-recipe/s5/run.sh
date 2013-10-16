#!/bin/bash

renice 20 $$

# Copyright Ondrej Platek Apache 2.0
# based heavily on copyrighted 2012 Vassil Panayotov recipe 
# at egs/voxforge/s5/run.sh(Apache 2.0)

. ./path.sh

# If you have cluster of machines running GridEngine you may want to
# change the train and decode commands in the file below
. ./cmd.sh

# Load few variables for changing the parameters of the training
. ./conf/train_conf.sh


# Copy the configuration files to exp directory.
# Write into the exp WARNINGs if reusing settings from another experiment!
local/save_check_conf.sh || exit 1;

# With save_check_conf.sh it ask about rewriting the data directory
if [ ! "$(ls -A data 2>/dev/null)" ]; then

  local/vystadial_data_prep.sh --every_n $everyN ${DATA_ROOT} || exit 1
  
  # Prepare the lexicon, language model and various phone lists
  # DISABLED because of swig: Sequitor model: 
  # Pronunciations for OOV words are obtained using a pre-trained Sequitur model
  local/vystadial_prepare_dict.sh || exit 1 
  
  # Prepare data/lang and data/local/lang directories read it IO param describtion
  utils/prepare_lang.sh data/local/dict 'OOV' data/local/lang data/lang || exit 1
  
  # Prepare G.fst
  local/vystadial_format_data.sh || exit 1 
fi 
# end of generating data directory
  
  
# With save_check_conf.sh it ask about rewriting the ${MFCC_DIR} directory
if [ ! "$(ls -A ${MFCC_DIR} 2>/dev/null)" ]; then

  # Creating MFCC features and storing at ${MFCC_DIR} (Could be large).
  for x in train $test_sets ; do 
    steps/make_mfcc.sh --mfcc-config conf/mfcc.conf --cmd \
      "$train_cmd" --nj $njobs data/$x exp/make_mfcc/$x ${MFCC_DIR} || exit 1;
    # CMVN does not have sense for us
    steps/compute_cmvn_stats.sh data/$x exp/make_mfcc/$x ${MFCC_DIR} || exit 1;
  done
fi

# Train monophone models
# If the monoTrainData is specified Train on data T; |T|==monoTrainData 
if [ -z $monoTrainData ] ; then 
    steps/train_mono.sh --run-cmn $cmn --nj $njobs --cmd "$train_cmd" data/train data/lang exp/mono || exit 1;
else
    utils/subset_data_dir.sh data/train $monoTrainData data/train.sub  || exit 1;
    steps/train_mono.sh --run-cmn $cmn --nj $njobs --cmd "$train_cmd" data/train.sub data/lang exp/mono || exit 1;
fi
 
# Monophone decoding
for data_dir in $test_sets ; do
 utils/mkgraph.sh --mono data/lang_$data_dir exp/mono exp/mono/graph || exit 1
 # note: steps/decode.sh calls the command line once for each test, 
 # and afterwards averages the WERs into (in this case exp/mono/decode/)
 steps/decode.sh --run-cmn $cmn --config conf/decode.config --nj $njobs --cmd "$decode_cmd" \
   exp/mono/graph data/$data_dir exp/mono/decode_$data_dir
done
 
# Get alignments from monophone system.
steps/align_si.sh --run-cmn $cmn --nj $njobs --cmd "$train_cmd" \
  data/train data/lang exp/mono exp/mono_ali || exit 1;

# train tri1 [first triphone pass]
steps/train_deltas.sh --run-cmn $cmn --cmd "$train_cmd" \
  $pdf $gauss data/train data/lang exp/mono_ali exp/tri1 || exit 1;
 
# decode tri1
for data_dir in $test_sets ; do
 utils/mkgraph.sh data/lang_$data_dir exp/tri1 exp/tri1/graph || exit 1;
 steps/decode.sh --run-cmn $cmn --config conf/decode.config --nj $njobs --cmd "$decode_cmd" \
   exp/tri1/graph data/$data_dir exp/tri1/decode_$data_dir
done
 
draw-tree data/lang/phones.txt exp/tri1/tree | dot -Tsvg -Gsize=8,10.5 
  
#align tri1 
steps/align_si.sh --run-cmn $cmn --nj $njobs --cmd "$train_cmd" \
  --use-graphs true data/train data/lang exp/tri1 exp/tri1_ali || exit 1;
  
# train tri2a [delta+delta-deltas]
steps/train_deltas.sh --run-cmn $cmn --cmd "$train_cmd" $pdf $gauss \
  data/train data/lang exp/tri1_ali exp/tri2a || exit 1;
  
# decode tri2a
for data_dir in $test_sets ; do
 utils/mkgraph.sh data/lang_$data_dir exp/tri2a exp/tri2a/graph
 steps/decode.sh --run-cmn $cmn --config conf/decode.config --nj $njobs --cmd "$decode_cmd" \
  exp/tri2a/graph data/$data_dir exp/tri2a/decode_$data_dir
done
 
# train and decode tri2b [LDA+MLLT]
steps/train_lda_mllt.sh --run-cmn $cmn --cmd "$train_cmd" $pdf $gauss \
  data/train data/lang exp/tri1_ali exp/tri2b || exit 1;
for data_dir in $test_sets ; do
 utils/mkgraph.sh data/lang_$data_dir exp/tri2b exp/tri2b/graph
 steps/decode.sh --run-cmn $cmn --config conf/decode.config --nj $njobs --cmd "$decode_cmd" \
  exp/tri2b/graph data/$data_dir exp/tri2b/decode_$data_dir
done
 
# Align all data with LDA+MLLT system (tri2b)
steps/align_si.sh --run-cmn $cmn --nj $njobs --cmd "$train_cmd" \
    --use-graphs true data/train data/lang exp/tri2b exp/tri2b_ali || exit 1;
  
# #  Do MMI on top of LDA+MLLT.
steps/make_denlats.sh --run-cmn $cmn --nj $njobs --cmd "$train_cmd" \
   data/train data/lang exp/tri2b exp/tri2b_denlats || exit 1;
steps/train_mmi.sh --run-cmn $cmn data/train data/lang exp/tri2b_ali exp/tri2b_denlats exp/tri2b_mmi || exit 1;
for data_dir in $test_sets ; do
 steps/decode.sh --run-cmn $cmn --config conf/decode.config --iter 4 --nj $njobs --cmd "$decode_cmd" \
  exp/tri2b/graph data/$data_dir exp/tri2b_mmi/decode_it4_$data_dir
 steps/decode.sh --run-cmn $cmn --config conf/decode.config --iter 3 --nj $njobs --cmd "$decode_cmd" \
  exp/tri2b/graph data/$data_dir exp/tri2b_mmi/decode_it3_$data_dir
done
 
# Do the same with boosting. train_mmi_boost is a number e.g. 0.05
steps/train_mmi.sh --run-cmn $cmn --boost ${train_mmi_boost} data/train data/lang \
   exp/tri2b_ali exp/tri2b_denlats exp/tri2b_mmi_b${train_mmi_boost} || exit 1;
for data_dir in $test_sets ; do
 steps/decode.sh --run-cmn $cmn --config conf/decode.config --iter 4 --nj $njobs --cmd "$decode_cmd" \
  exp/tri2b/graph data/$data_dir exp/tri2b_mmi_b${train_mmi_boost}/decode_it4_$data_dir || exit 1;
 steps/decode.sh --run-cmn $cmn --config conf/decode.config --iter 3 --nj $njobs --cmd "$decode_cmd" \
  exp/tri2b/graph data/$data_dir exp/tri2b_mmi_b${train_mmi_boost}/decode_it3_$data_dir || exit 1;
done

# Do MPE.
steps/train_mpe.sh --run-cmn $cmn data/train data/lang exp/tri2b_ali exp/tri2b_denlats exp/tri2b_mpe || exit 1;
for data_dir in $test_sets ; do
 steps/decode.sh --run-cmn $cmn --config conf/decode.config --iter 4 --nj $njobs --cmd "$decode_cmd" \
  exp/tri2b/graph data/$data_dir exp/tri2b_mpe/decode_it4_$data_dir || exit 1;
 steps/decode.sh --run-cmn $cmn --config conf/decode.config --iter 3 --nj $njobs --cmd "$decode_cmd" \
  exp/tri2b/graph data/$data_dir exp/tri2b_mpe/decode_it3_$data_dir || exit 1;
done


echo "Successfully trained and evaluated all the experiments"

if [ -f local/backup.sh ]; then 
    local/backup.sh
fi
