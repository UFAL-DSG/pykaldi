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


from pykaldi.utils import load_wav, wst2dict
from pykaldi.decoders import PyGmmLatgenWrapper
import sys

# FIXME todo measure time of decode resp decode_once through profiler


def write_decoded(f, wav_name, word_ids, wst):
    print 'DEBUG', word_ids
    if wst is not None:
        decoded = [wst[str(w)] for w in word_ids]
    else:
        decoded = [str(w) for w in word_ids]
    line = ' '.join([wav_name] + decoded + ['\n'])
    print >> sys.stderr, 'DEBUG %s' % line
    f.write(line)


def decode(d, pcm):
    frame_len = (2 * audio_batch_size)  # 16-bit audio so 1 sample = 2 chars
    it = (len(pcm) / frame_len)
    print >> sys.stderr, 'NUMBER of audio input chunks: %d' % it
    decoded_frames = 0
    for i in xrange(it):
        audio_chunk = pcm[i * frame_len:(i + 1) * frame_len]
        d.frame_in(audio_chunk)
        dec_t = d.decode(max_frames=10)
        while dec_t > 0:
            decoded_frames += dec_t
            dec_t = d.decode(max_frames=10)
    d.prune_final()
    # decoded = d.get_nbest(n=10)
    # prob, words = decoded[0]
    fst_lat = d.get_lattice()
    fst_lat.write('python_debug_last.fst')  # the same fst OK
    p = fst_lat.shortest_path()  # DEBUG remove
    words = []
    for state in p.states:
        for arc in state.arcs:
            words.append(arc.ilabel)
    return words


def decode_wrap(argv, audio_batch_size, wav_paths, file_output, wst=None):
    d = PyGmmLatgenWrapper()
    d.setup(argv)
    for wav_name, wav_path in wav_paths:
        # 16-bit audio so 1 sample_width = 2 chars
        pcm = load_wav(wav_path, def_sample_width=2, def_sample_rate=16000)
        d.reset(keep_buffer_data=False)
        word_ids = decode(d, pcm)
        write_decoded(file_output, wav_name, word_ids, wst)


if __name__ == '__main__':
    audio_scp, audio_batch_size, dec_hypo = sys.argv[1], int(sys.argv[2]), sys.argv[3]
    argv = sys.argv[4:]
    print >> sys.stderr, 'Python args: %s' % str(sys.argv)

    # Try to locate and extract wst argument
    wst = None
    for a in argv:
        if a.endswith('words.txt'):
            wst = wst2dict(a)

    # open audio_scp, decode and write to dec_hypo file
    with open(audio_scp, 'rb') as r:
        with open(dec_hypo, 'wb') as w:
            lines = r.readlines()
            scp = [tuple(line.strip().split(' ', 1)) for line in lines]
            decode_wrap(argv, audio_batch_size, scp, w, wst)
