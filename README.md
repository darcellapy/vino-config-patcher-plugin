# vino-config-patcher-plugin
A Wii U Aroma plugin that patches a TVii config file with a custom one from your SD card to redirect it's urls.

The file `vino_config.txt` should be placed in the directory: `sd:/TVii/vino_config.txt`. Failure to do so will result in a console crash with a neat little message telling you to basically do what you should've already been doing. This is a fully foolproof plugin for sure!

# Where do I get a vino_config.txt?
You can use the example one provided in the source, rename it to `vino_config.txt` of course, put it in the `TVii` folder at the root of your SD card, and then edit it to change settings and the urls TVii redirects to.

# Note
Not every website will work with TVii. Some websites may even have certificates that the Wii U will not like and prevent loading them.
