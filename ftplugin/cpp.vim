" .cppと.hをトグルする
function! ToggleSourceHeader()
    let extension = expand("%:e")
    if extension == "cpp"
        :e %:r.h
    elseif extension == "h"
        :e %:r.cpp
    endif
endfunction

command! -nargs=0 ToggleSH call ToggleSourceHeader()

nnoremap <buffer> <C-s> :call ToggleSourceHeader()

" make!
nnoremap <buffer> <Leader>mm :make vc-release
nnoremap <buffer> <Leader>ms :make source
