1.Installation
We need to install ImageMagika which is in ./vendor or ./ before use. Remember to add it to the system path.



2.File formats:
1) All images use JPG format.
2) All videos use AVI format, while JPEG enconding are recommended for complete functionality. MPEG encoding will fail the random-video-frame-access function.


3.Configuration
All the configurable option can be made in ./config/config.ini. Most of the items are introspective. Here are some needing explaination.

enableDebug: 0 means normal, 1 means debug mode. In debug mode, the app will show logoletSize*logoletSize region instead of logo region.

logoletSize: minimum region size. For now, make it 12. I have not test about making it another value.

mode: set to 0 means detecting mode, 1 means generating neural network input file.

Items in Detector section should remain the same for normal running. Of course, change it if you know what it means.

You know the rest. Have fun!