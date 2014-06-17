#!flask/bin/python
# encoding: utf-8
"""
Presents live demo of PyOnlineLatgenRecogniser decoding.
The audio recording requires working PyAudio.
Requieres arguments specifying AM, HCLG graph, etc ...
"""
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
from __future__ import unicode_literals

import pyaudio
from kaldi.decoders import PyOnlineLatgenRecogniser
from kaldi.utils import wst2dict, lattice_to_nbest
import sys
from flask import Flask, jsonify, render_template

CHANNELS, RATE, FORMAT = 1, 16000, pyaudio.paInt16
dec_frames = 0
d = PyOnlineLatgenRecogniser()
d.reset()
app = Flask(__name__)


@app.route('/get_decoded')
def get_dec():
    global dec_frames
    new_frames = d.decode(max_frames=10)
    while new_frames > 0:
        dec_frames += new_frames
        new_frames = d.decode(max_frames=10)
    resp = jsonify({'dec_frames': dec_frames})
    resp.status_code = 200
    return resp


@app.route('/get_lattice')
def get_lat():
    global dec_frames
    d.prune_final()
    lik, lat = d.get_lattice()
    nbest = lattice_to_nbest(lat, n=10)
    nbest_s = '\n'.join(["%0.3f %s" % (prob, ' '.join([wst[i] for i in ids])) for (prob, ids) in nbest])
    dec_frames, result = 0, jsonify(nbest=nbest_s, dec_frames=dec_frames)
    print 'DEBUG', dec_frames
    d.reset(keep_buffer_data=False)
    return result


def get_audio_callback():
    """Returns a callback - function which handle incomming audio"""
    def frame_in(in_data, frame_count, time_info, status):
        d.frame_in(in_data)
        return in_data, pyaudio.paContinue
    return frame_in


@app.route('/')
def index():
    return render_template('index.html')


audio_batch_size, wst_path = int(sys.argv[1]), sys.argv[2]
argv = sys.argv[3:]
print >> sys.stderr, 'Python args: %s' % str(sys.argv)

wst = wst2dict(wst_path)

d.setup(argv)
pin = pyaudio.PyAudio()
stream = pin.open(format=FORMAT, channels=CHANNELS,
                 rate=RATE, input=True, frames_per_buffer=audio_batch_size,
                            stream_callback=get_audio_callback())

app.run(host='0.0.0.0', debug=True)
