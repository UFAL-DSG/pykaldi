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


from pykaldi.binutils.utils import parse_config_from_arguments, make_dir, \
    build_reference, wst2dict, int_to_txt, compact_hyp, \
    config_is_yes, load_wav
from pykaldi.exceptions import PyKaldiError
from pykaldi.binutils import ffibin, libbin
from pykaldi.decoders import OnlineDecoder, DecoderCloser
import time


def run_online_dec(pcm, argv, samples_per_frame):
    with DecoderCloser(OnlineDecoder(argv)) as d:
        # using 16-bit audio so 1 sample = 2 chars
        frame_len = (2 * samples_per_frame)
        # Pass the audio data to decoder at once
        for i in range(len(pcm) / frame_len):
            frame = pcm[i * frame_len:(i + 1) * frame_len]
            d.frame_in(frame, samples_per_frame)
        # Extract the hypothesis in form of word ids
        word_ids, prob = d.finish_decoding()
        return word_ids, prob


def recreate_dec(argv, samples_per_frame, wav_paths, file_output):
    for wav_name, wav_path in wav_paths:
        pcm = load_wav(wav_path)
        print 'Processing utterance %s.' % wav_name
        word_ids, prob = run_online_dec(pcm, argv, samples_per_frame)

        line = [wav_name] + [str(word_id) for word_id in word_ids] + ['\n']
        file_output.write(' '.join(line))
        print 'Result for %s written.' % wav_name


def no_finish(argv, samples_per_frame, wav_paths, file_output, wst=None, duration=10):
    with DecoderCloser(OnlineDecoder(argv)) as d:
        for wav_name, wav_path in wav_paths:
            print 'Processing utterance %s.' % wav_name
            pcm = load_wav(wav_path)
            # using 16-bit audio so 1 sample = 2 chars
            frame_len = (2 * samples_per_frame)
            # Pass the audio data to decoder at once
            tot_ids = []
            for i in xrange(len(pcm) / frame_len):
                frame = pcm[i * frame_len:(i + 1) * frame_len]
                d.frame_in(frame, samples_per_frame)
            start = time.time()
            # run the backward search only for limited amount of time
            while (time.time() - start) < duration:
                    word_ids, prob = d.decode()
                    if wst is not None and len(word_ids) > 0:
                        print [wst[word_id] for word_id in word_ids]
                    # FIXME usage of decode decrease performance if not sleep
                    # because of hardcoded beem update
                    time.sleep(0.1)
                    tot_ids.extend(word_ids)
            print 'finish decoding'
            # finish decoding
            word_ids, prob = d.finish_decoding()
            if wst is not None and len(word_ids) > 0:
                print [wst[word_id] for word_id in word_ids]
            tot_ids.extend(word_ids)

            line = [wav_name] + [str(word_id) for word_id in tot_ids] + ['\n']
            file_output.write(' '.join(line))
            print 'Result for %s written.' % wav_name


def decode_once(argv, samples_per_frame, wav_paths, file_output, wst=None):
    with DecoderCloser(OnlineDecoder(argv)) as d:
        for wav_name, wav_path in wav_paths:
            print 'Processing utterance %s.' % wav_name
            pcm = load_wav(wav_path)
            # using 16-bit audio so 1 sample = 2 chars
            frame_len = (2 * samples_per_frame)
            # Pass the audio data to decoder at once
            for i in xrange(len(pcm) / frame_len):
                frame = pcm[i * frame_len:(i + 1) * frame_len]
                d.frame_in(frame, samples_per_frame)
            # Extract the hypothesis at once in form of word ids
            word_ids, prob = d.finish_decoding()
            if wst is not None:
                # Debug print
                print [wst[word_id] for word_id in word_ids]
            # Store the results to file
            line = [wav_name] + [str(word_id) for word_id in word_ids] + ['\n']
            file_output.write(' '.join(line))
            print 'Result for %s written.' % wav_name


def decode_zig_zag(argv, samples_per_frame, wav_paths, file_output, wst=None):
    with DecoderCloser(OnlineDecoder(argv)) as d:
        for wav_name, wav_path in wav_paths:
            print 'Processing utterance %s.' % wav_name
            pcm = load_wav(wav_path)
            # using 16-bit audio so 1 sample = 2 chars
            frame_len = (2 * samples_per_frame)
            # FIXME probably should use logarithm when using for real
            tot_prob = 1  # total probability for whole recording
            tot_ids = []
            it = len(pcm) / frame_len
            print 'NUMBER of iterations: %s' % it
            for i in xrange(it):
                frame = pcm[i * frame_len:(i + 1) * frame_len]
                d.frame_in(frame, samples_per_frame)
                word_ids, prob = d.decode()
                tot_ids.extend(word_ids)
                tot_prob *= prob
                if wst is not None and len(word_ids) > 0:
                    # Debug print
                    print [wst[word_id] for word_id in word_ids]
            # finish decoding
            word_ids, prob = d.finish_decoding()
            if wst is not None and len(word_ids) > 0:
                # Debug print
                print [wst[word_id] for word_id in word_ids]
            tot_ids.extend(word_ids)
            tot_prob *= prob
            # Store the results to file
            line = [wav_name] + [str(word_id) for word_id in tot_ids] + ['\n']
            file_output.write(' '.join(line))
            print 'Result for %s written. Probability %.2f' % (wav_name, tot_prob)


def run_python_online(config):

    c = config['online-python']
    if not config_is_yes(c, 'run'):
        print 'Skipping running run_python_online'
        return
    argv = ['--config=%(config)s' % c,
            c['model'], c['hclg'],
            config['wst'], '%(silent_phones)s' % c]
    samples_per_frame = int(c['samples_per_frame'])

    with open(config['wav_scp'], 'rb') as r:
        lines = r.readlines()
        scp = [tuple(line.strip().split(' ', 1)) for line in lines]

    wst_dict = wst2dict(config['wst'], intdict=True)

    with open(c['trans'], 'wb') as w:
        if c['type'] == 'recreate_dec':
            recreate_dec(argv, samples_per_frame, scp, w)
        elif c['type'] == 'decode_once':
            decode_once(argv, samples_per_frame, scp, w, wst_dict)
        elif c['type'] == 'decode_zig_zag':
            decode_zig_zag(argv, samples_per_frame, scp, w, wst_dict)
        elif c['type'] == 'no_finish':
            no_finish(argv, samples_per_frame, scp, w, wst_dict)
        else:
            raise Exception('Unknown type of online-python decoding')


def compute_wer(ffi, werlib, config):
    '''Settings and arguments based on /ha/work/people/oplatek/kaldi-trunk/egs/kaldi-
    vystadial-recipe/s5/local/shore.sh
    | compute-wer --text --mode=present ark:exp/tri2a/decode/scoring/test_filt.txt ark,p:- >&
    exp/tri2a/decode/wer_15'''

    c = config['wer-compute']
    if not config_is_yes(c, 'run'):
        print 'Skipping running WER'
        return

    if config_is_yes(c, 'build_ref'):
        build_ref_scp, refer = config['wav_scp'], c['reference']
        build_reference(build_ref_scp, refer)
    else:
        print 'Not creating reference'

    wst_dict = wst2dict(config['wst'])

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
    compute_wer(ffibin, libbin, config)
