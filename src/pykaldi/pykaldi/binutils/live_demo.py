#!/usr/bin/env python
# encoding: utf-8
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


import pyaudio
from pykaldi.decoders import OnlineDecoder
from pykaldi.binutils.utils import wst2dict
import sys


def setup_pyaudio(samples_per_frame):
    p = pyaudio.PyAudio()
    stream = p.open(format=p.get_format_from_width(pyaudio.paInt32),
                    channels=1,
                    rate=16000,
                    input=True,
                    output=True,
                    frames_per_buffer=samples_per_frame)
    return (p, stream)


def teardown_pyaudio(p, stream):
    stream.stop_stream()
    stream.close()
    p.terminate()
    p, stream = None, None


def run_live(argv, stream, samples_per_frame, wst=None, duration=20):
    '''Runs live demo for number of seconds speficied by duration.'''
    d = OnlineDecoder(argv)
    num_frames = duration * (16000 / samples_per_frame)
    for i in xrange(num_frames):
        frame = stream.read(2 * samples_per_frame)
        d.frame_in(frame, len(frame) / 2)
        word_ids, prob = d.decode()
        if wst is None:
            hyp = word_ids
        else:
            hyp = [wst[word_id] for word_id in word_ids]
        if len(word_ids) > 0:
            print '%f from %d sec: num_words %d, ids: %s' % (
                (float(i) / num_frames) * duration, duration, len(hyp), str(hyp))
        assert prob == 1.0, 'Is probability measure implemented now?'
    d.close()

if __name__ == '__main__':
    ''' example parameters:
    export DATA=online-data/models/tri2a
    ./live_demo.py --rt-min=0.5 --rt-max=10000.0 --max-active=4000 --beam=12.0 --acoustic-scale=0.0769 $DATA/model $DATA/HCLG.fst $DATA/words.txt 1:2:3:4:5
    '''
    samples_per_frame = 2120
    argv = sys.argv[1:]
    wst = None
    for arg in argv:
        if 'words.txt' in arg:
            wst = wst2dict(arg, intdict=True)
    p, stream = setup_pyaudio(samples_per_frame)
    run_live(argv, stream, samples_per_frame, wst=wst)
    teardown_pyaudio(p, stream)
