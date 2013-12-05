" TINAのログファイル用設定
augroup tinalog 
au!
    au  BufNewFile,BufRead *Log.txt     setfiletype tinalog
    au  BufNewFile,BufRead *Log.txt     set tabstop=8
augroup END
