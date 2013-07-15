#!/usr/bin/env python
# Copyright (c) 2013, Ondrej Platek, Ufal MFF UK <oplatek@ufal.mff.cuni.cz>
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#  http://www.apache.org/licenses/LICENSE-2.0
#
# THIS CODE IS PROVIDED *AS IS* BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
# KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION ANY IMPLIED
# WARRANTIES OR CONDITIONS OF TITLE, FITNESS FOR A PARTICULAR PURPOSE,
# MERCHANTABLITY OR NON-INFRINGEMENT.
# See the Apache 2 License for the specific language governing permissions and
# limitations under the License. #


from decoding_pipeline_utils import parse_config_from_arguments, make_dir, build_reference, wst2dict, int_to_txt, compact_hyp, PyKaldiError
from pykaldi.binutils import ffibin, libbin
import numpy as np


def run_mfcc(ffi, mfcclib, config):
    '''Settings and arguments based on /ha/work/people/oplatek/kaldi-trunk/egs/kaldi-
    vystadial-recipe/s5/steps/make_mfcc.sh'''
    c = config['mfcc']
    mfcc_args = ['python-compute-mfcc-feat', '--config=%(config)s' % c,
                 'scp:%(wav_scp)s' % config, 'ark,scp:%(ark)s,%(scp)s' % c]
    try:
        mfcc_argkeepalive = [ffi.new("char[]", arg) for arg in mfcc_args]
        mfcc_argv = ffi.new("char *[]", mfcc_argkeepalive)
        retcode = mfcclib.compute_mfcc_feats_like_main(
            len(mfcc_args), mfcc_argv)
        if retcode != 0:
            raise PyKaldiError(retcode)
    except Exception as e:
        print 'Failed running mfcc!'
        raise e


def run_cmvn(ffi, mfcclib, config):
    c = config['cmvn']
    mfcc_args = ['python-compute-cmvn-stats', '--spk2utt=%(spk2utt)s' % c,
                 'scp:%s' % config['mfcc']['scp'], 'ark,scp:%(ark)s,%(scp)s' % c]
    try:
        mfcc_argkeepalive = [ffi.new("char[]", arg) for arg in mfcc_args]
        mfcc_argv = ffi.new("char *[]", mfcc_argkeepalive)
        retcode = mfcclib.compute_cmvn_stats_like_main(
            len(mfcc_args), mfcc_argv)
        if retcode != 0:
            raise PyKaldiError(retcode)
    except Exception as e:
        print 'Failed running cmvn!'
        raise e


def run_decode(ffi, decodelib, config):
    '''Settings and arguments based on /ha/work/people/oplatek/kaldi-trunk/egs/kaldi-
    vystadial-recipe/s5/steps/decode.sh'''
    c = config['latgen-decode']
    # feats for delta not lda
    # feats = 'ark,s,cs:apply-cmvn --norm-vars=false --utt2spk=ark:%(utt2spk)s scp:%(cmvn)s ' % c
    # feats += 'scp:%s ark:- | add-deltas ark:- ark:- |' % config['mfcc']['scp']
    # or without CMVN
    feats = 'ark,s,cs:add-deltas scp:%s ark:- |' % config['mfcc']['scp']
    decode_args = ['python-gmm-latgen-faster', '--config=%(config)s' % c,
                   '--word-symbol-table=%(wst)s' % config,
                   c['model'], c['hclg'], feats,
                   'ark:|gzip -c > %(lattice)s' % c]
    try:
        decode_argkeepalive = [ffi.new("char[]", arg) for arg in decode_args]
        decode_argv = ffi.new("char *[]", decode_argkeepalive)
        retcode = decodelib.gmm_latgen_faster_like_main(
            len(decode_args), decode_argv)
        if retcode != 0:
            raise PyKaldiError(retcode)
        print 'Running decode finished!'
    except Exception as e:
        print 'Failed running decode!'
        raise e


def run_bestpath(ffi, bestpathlib, config):
    ''' Settings and arguments based on /ha/work/people/oplatek/kaldi-trunk/egs/kaldi-
    vystadial-recipe/s5/local/shore.sh'''
    c = config['best-path']
    bestpath_args = ['bestpath_unsed', '--config=%(config)s' % c, '--word-symbol-table=%(wst)s' % config,
                     'ark:gunzip -c %s|' % config['latgen-decode']['lattice'],
                     'ark,t:%(trans)s' % c]
    try:
        bestpath_argkeepalive = [ffi.new("char[]", arg)
                                 for arg in bestpath_args]
        bestpath_argv = ffi.new("char *[]", bestpath_argkeepalive)
        retcode = bestpathlib.lattice_best_path_like_main(
            len(bestpath_args), bestpath_argv)
        if retcode != 0:
            raise PyKaldiError(retcode)
    except Exception as e:
        print 'Failed running bestpath!'
        raise e


def run_online(ffi, onlinelib, config):
    ''' Based on kaldi-trunk/egs/voxforge/online_demo/run.sh'''
    c = config['online-decode']
    ldc = config['latgen-decode']
    online_args = ['python-online-wav-gmm-decode-faster',
                   '--config=%(config)s' % c, 'scp:%(wav_scp)s' % config,
                   ldc['model'], ldc['hclg'], config['wst'], '1:2:3:4:5',
                   'ark,t:%(trans)s' % c, 'ark,t:%(align)s' % c]
    try:
        online_argkeepalive = [ffi.new("char[]", arg) for arg in online_args]
        online_argv = ffi.new("char *[]", online_argkeepalive)
        retcode = onlinelib.online_wav_gmm_decode_faster_like_main(
            len(online_args), online_argv)
        if retcode != 0:
            raise PyKaldiError(retcode)
    except Exception as e:
        print 'Failed running online!'
        raise e


def run_python_online(config):
    from pykaldi.decoders.kaldi_decoders import OnlineDecoder
    from pykaldi.decoders.kaldi_decoders_test import load_wav

    c, ldc = config['online-python'], config['latgen-decode']
    argv = ['--config=%(config)s' % c,
            ldc['model'], ldc['hclg'],
            config['wst'], '1:2:3:4:5']
    samples_per_frame = int(c['samples_per_frame'])

    with open(config['wav_scp'], 'rb') as r:
        lines = r.readlines()
        scp = [tuple(line.strip().split(' ', 1)) for line in lines]

    with open(c['trans'], 'wb') as w:
        for wav_name, wav_path in scp:
            d = OnlineDecoder(argv)
            print 'Processing utterance %s.' % wav_name
            pcm = load_wav(wav_path)
            word_ids = np.array([], dtype=np.int32)
            # using 16-bit audio so 1 sample = 2 chars
            frame_len = (2 * samples_per_frame)
            # Pass the audio data to decoder at once
            for i in range(len(pcm) / frame_len):
                frame = pcm[i * frame_len:(i + 1) * frame_len]
                d.frame_in(frame, samples_per_frame)
            d.finish_input()
            # Extract the hypothesis in form of word ids
            while d.decode():
                num_words, full_hyp = d.prepare_hyp()
                if num_words > 0:
                    prop, new_ids = d.get_hypothesis(num_words)
                    word_ids = np.concatenate([word_ids, new_ids])
            # Decode last hypothesis
            num_words, full_hyp = d.prepare_hyp()
            if num_words > 0:
                prop, new_ids = d.get_hypothesis(num_words)
                word_ids = np.concatenate([word_ids, new_ids])
            # Store the results to file
            line = [wav_name]
            line.extend([str(word_id) for word_id in word_ids])
            line.append('\n')
            w.write(' '.join(line))
            print 'Result for %s written.' % wav_name
            d.close()  # DO NOT FORGET TO CLOSE THE DECODER!


def compute_wer(ffi, werlib, config):
    '''Settings and arguments based on /ha/work/people/oplatek/kaldi-trunk/egs/kaldi-
    vystadial-recipe/s5/local/shore.sh
    | compute-wer --text --mode=present ark:exp/tri2a/decode/scoring/test_filt.txt ark,p:- >&
    exp/tri2a/decode/wer_15'''
    c = config['wer-compute']
    wst_dict = wst2dict(config['wst'])
    wav_scp, refer = config['wav_scp'], c['reference']

    build_reference(wav_scp, refer)

    for hyp in c['hypothesis']:
        # preprocessing the hypothesis
        hyp_txt, hyp_compact = hyp + '.txt', hyp + '.compact'
        int_to_txt(hyp, hyp_txt, wst_dict, unknown_symbol='\<UNK\>')
        compact_hyp(hyp_txt, hyp_compact)

        wer_args = ['python-compute-wer', '--config=%(config)s' % c,
                    'ark:%(reference)s' % c, 'ark:%s' % hyp_compact]
        wer_argkeepalive = [ffi.new("char[]", arg) for arg in wer_args]
        wer_argv = ffi.new("char *[]", wer_argkeepalive)
        retcode = werlib.compute_wer_like_main(len(wer_args), wer_argv)
        if retcode != 0:
            raise PyKaldiError(retcode)


if __name__ == '__main__':
    config = parse_config_from_arguments()
    make_dir(config['decode_dir'])

    run_python_online(config)
    print 'running PYTHON ONLINE finished'
    ### Evaluating experiments
    compute_wer(ffibin, libbin, config)
    print 'running WER finished'
