String Search - Qt applications
====================================================
Description
-----------
Created a multi-threaded analogue of GREP and directory indexing tools with UI for 
finding string occurences in the selected folder

Install Qt 5 on Ubuntu
---------------------
Open a terminal. Type the following command and hit Enter:
```bash
sudo apt-get install qt5-default
```

Run application
----------------------
Open a terminal. Type the following command and hit Enter:
Steps:
```bash
git clone https://github.com/ShSamariddin/StringSearch.git
cd StringSearch
chmod u+x linux.sh
./linux.sh
```
## Preview
![preview.gif](https://i.loli.net/2020/09/16/Jrv5iVZK8PshQpe.gif)

### Screenshot
<img src="https://i.loli.net/2020/09/16/raL86U32ljuDsn4.png" alt="PrSubtitle" width="650" />

### Features
* monitor file changes
* quick search by pre-counting all triggers and searching by them
* multithreading
* User interface
* automatically rebuilding indexes when file is changed
* stop indexing or searching