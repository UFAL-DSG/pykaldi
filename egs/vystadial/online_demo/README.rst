Running the example Pykaldi scripts
===================================

Summary
-------
The `<pykaldi-latgen-faster-decoder.py>`_
demonstrates how to use the class ``PyOnlineLatgenRecogniser``,
which takes audio on the input and outputs the decoded lattice.
There are also the C++ and ``gmm-latgen-faster`` demos.
All three demos produce the same results.
Currently, the test data are for Czech language only.

Dependencies
------------
* For running all 
* For inspecting the saved lattices you need `dot` binary 
  from `Graphviz <http://www.graphviz.org/Download..php>`_ library.
* For running the live demo you need ``pyaudio`` package.

Running the example scripts
---------------------------

.. code-block:: bash

    make online-latgen-recogniser

* Run the test ``src/onl-rec/onl-rec-latgen-recogniser-test`` for ``OnlineLatgenRecogniser``
  which shows C++ example of how to use the recogniser.
  The same data, AM a LM are used as for ``make pyonline-latgen-recogniser``.
  The pretrained Language (LM) and Acoustic (AM) models are used.
  The data as well as the models are downloaded from our server.

.. code-block:: bash

    make pyonline-latgen-recogniser

* Run the decoding with ``PyOnlineFasterRecogniser``. 
  Example Python script ``pykaldi-online-latgen-recogniser.py`` shows 
  ``PyOnlineFasterRecogniser`` decoding  on small test set.
  The same pretrained Language (LM) and Acoustic (AM) models.

.. code-block:: bash

    make gmm-latgen-faster

* Run the decoding with Kaldi ``gmm-latgen-faster`` executable wrapped in `<run_gmm-latgen-faster.sh>`_.
  This is the reference executable for 
  The same data, AM a LM are used as for ``make pyonline-latgen-recogniser``.
  We use this script as reference.

.. code-block:: bash

    make live

* The simple live demo should decode speech from your microphone.
  It uses the pretrained AM and LM and wraps `<live-demo.py>`_. 
  The ``pyaudio`` package is used for capturing the sound from your microphone.
  We were able to use it under `Ubuntu 12.10` and Python 2.7, but we guarantee nothing on your system.
