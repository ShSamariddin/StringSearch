cd src
qmake StringSearch.pro
make
./StringSearch
rm *.o
rm moc_*
rm ui_*
rm StringSearch
#xdg-open .

