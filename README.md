About This Code
===============

This code takes a HU01 compressed email message as produced by Microsoft Windows Live Mail service and decompresses it.
I produced it by reverse engineering the decompression routine from the Windows Live Mail desktop application sporadically over a couple of years (probably about a weeks work in total).

The code is AWFUL, but it seems to work with all the message data I've thrown at it.

How Do I Use It?
================

Compile the code my running "make", then try it out using the following command

    ./hu01 test.hu01 test.plain

