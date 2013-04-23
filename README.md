logo_detection
==============
There is a development version and runtime version in the repository, both work on windows only.
Development version is made on CodeBlocks 10.5. Newer versions have not been tested yet.

1.Installation
We need to install ImageMagika which is in ./vendor or ./ before use. Remember to add it to the system path.

2.File formats:
1) All images use JPG format.
2) All videos use AVI format, while JPEG enconding are recommended for complete functionality. 
MPEG encoding will fail the random-video-frame-access function. The opencv version 2.3.1 has not address this issue yet. 


3.Configuration
All the configurable option can be made in ./config/config.ini. Most of the items are introspective. Here are some needing explaination.

enableDebug: 0 means normal, 1 means debug mode. In debug mode, the app will show logoletSize*logoletSize region instead of logo region.

logoletSize: minimum region size. For now, make it 12. I have not test about making it another value.

mode: set to 0 means detecting mode, 1 means generating neural network input file.

Items in Detector section should remain the same for normal running. Of course, change it if you know what it means.

4. How to run
Open ./config/config.ini, set inputDataFile to desired video input path and run the .exe.
There are 3 video files in ./data folder as easy test cases to use.
You know the rest. Have fun!

5. In Addition
There is also a paper in Chinese(it does have an English abstract though) which shows the work.  
It contains a number of pictures which should serve well as a reference to what the application looks like.

