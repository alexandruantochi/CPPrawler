# CPPrawler
C++ crawler


This application is a web crawler written in C++ for a school assignment. 

## Short description

The crawler thread reads the website links from the `HKLM/Software/WoW64Node/Webcrawler` and starts crawling them by a given depth. It sends the website size and name to a second thread which in turn sends this info to another processs backs up the data in the first file system partition found, with the exception of `C:/`.

*This can be modified to be used as a web crawler and can be used as is, but the complicated thread and process sync was written for didactical purposes, as they are redundant.*
