// -*- coding: utf-8 -*-
// Copyright 2014 Ondrej Platek 

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//  http://www.apache.org/licenses/LICENSE-2.0
//
// THIS CODE IS PROVIDED *AS IS* BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION ANY IMPLIED
// WARRANTIES OR CONDITIONS OF TITLE, FITNESS FOR A PARTICULAR PURPOSE,
// MERCHANTABLITY OR NON-INFRINGEMENT.
// See the Apache 2 License for the specific language governing permissions and
// limitations under the License.

#include <iostream>
#include <string>
#include <vector>

#include "base/kaldi-types.h"
#include "base/kaldi-math.h"
#include "feat/wave-reader.h"
#include "onl-rec/onl-rec-latgen-recogniser.h"
#include "onl-rec/onl-rec-utils.h"
#include "fstext/fstext-utils.h"

using namespace kaldi;
using std::string;

void print_ids(const std::string & utt, 
               const fst::SymbolTable *word_syms, 
               const std::vector<int> & word_ids) {
      std::cout << utt << ' ';
      for (size_t i = 0; i < word_ids.size(); i++) {
        std::string s = word_syms->Find(word_ids[i]);
        if (s == "")
          KALDI_WARN << "Word-id " << word_ids[i] << " not in symbol table.";
        std::cout << s << ' ';
      }
      std::cout << std::endl;
}

int main(int argc, char *argv[]) {
  try {
    std::vector<string> args(argv, argv+argc);
    if (args.size() < 3) {
      std::cerr << "Usage: " << args[0] << ": <wave_file> <wst> <GmmLatgenOptions>" 
        << std::endl;
      return -1;
    }

    fst::SymbolTable *word_syms = NULL;
    std::string word_syms_rxfilename = args[2];
    if (!(word_syms = fst::SymbolTable::ReadText(word_syms_rxfilename)))
      KALDI_ERR << "Could not read symbol table from file "
                << word_syms_rxfilename;

    SequentialTableReader<WaveHolder> wav_reader(args[1]); 

    OnlineLatgenRecogniser rec;
    rec.Setup(args.size() - 3, argv + 3);
    std::cerr << std::endl << " Recogniser initialized." << std::endl << std::endl;

    for (; !wav_reader.Done(); wav_reader.Next()) {
      const WaveData &wave_data = wav_reader.Value();
      int32 num_chan = wave_data.Data().NumRows(), this_chan = 0; 
      if(this_chan != num_chan -1) 
        KALDI_ERR << "Wave should have only one channel";
      SubVector<BaseFloat> waveform(wave_data.Data(), this_chan);

      // Pass commandline arguments except for the wave file name.
      // TODO ARGUMENT PARSER

      size_t decoded_frames = 0; 
      size_t decoded_now = 0; 
      size_t max_decoded = 10;
      // size_t chunk = 100; todo while loop and simulates chunks
      rec.FrameIn(&waveform);
      // std::cerr << std::endl << " Audio chunk buffered into decoder." << std::endl << std::endl;
      do {
        decoded_frames += decoded_now;
        decoded_now = rec.Decode(max_decoded);
      } while(decoded_now > 0);
      std::cerr << std::endl << " Audio chunk forward-decoded." << std::endl << std::endl;

      rec.FinalizeDecoding();
      
      std::vector<int> word_ids;
      BaseFloat lik;
      rec.GetBestPath(&word_ids, &lik);
      print_ids(wav_reader.Key(), word_syms, word_ids);

      // Writing the word posterior lattice to file
      // TODO separate config and parse options
      // double tot_lik = 0; 
      // fst::VectorFst<fst::LogArc> word_post_lat;
      // rec.GetLattice(&word_post_lat, &tot_lik);
      // std::ofstream logfile;
      // string fstname = utt.Key() + ".fst";
      // logfile.open(fstname.c_str());
      // word_post_lat.Write(logfile, fst::FstWriteOptions());
      // logfile.close();
      // std::cerr << std::endl << " Posterior lattice extracted." << std::endl << std::endl;
      // std::cerr << "Likelihood of the utterance is " << tot_lik << std::endl;

      bool reset_pipeline = true;  // True: The buffered unprocessed audio will be cleared
      rec.Reset(reset_pipeline);  // Ready for new utterance

      std::cerr << std::endl << " Recogniser resetted and prepared for next utterance." << std::endl << std::endl;
    } // end for
    delete word_syms; // will delete if non-NULL.
  } catch(const std::exception& e) {
    std::cerr << e.what();
    return -1;
  }

  return 0;
}
