This is interactive bookmarks for the bash shell with fuzzy filtering.

Add this to your .bashrc

```bash
bm() {
  bmarks
  cd $(cat ~/.bookmarks_res)
  pwd
}

```

'+' - add  bookmard <br>
'-' - remove <br>
'ENTER' - go to the directory <br>
'CTR-C' - abort <br>
