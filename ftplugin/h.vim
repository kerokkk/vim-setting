" .cppと.hをトグルする
function! s:ToggleSourceHeader()
    let extension = expand("%:e")
    if extension == "cpp"
        :e %:r.h
    elseif extension == "h"
        :e %:r.cpp
    endif
endfunction

command! -nargs=0 ToggleSH call s:ToggleSourceHeader()

nnoremap <buffer> <C-s> :ToggleSH<CR>

" make!
nnoremap <buffer> <Leader>mm :make vc-release
