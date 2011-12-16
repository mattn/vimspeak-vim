let s:libfile = substitute(expand('<sfile>'), '\.vim', '.dll', '')

function! VimSpeak(count, text) range
  let text = a:count == -1 ? a:text : join(a:firstline, a:lastline)
  call libcall(s:libfile, "Speak", iconv(text, &encoding, "utf-8"))
endfunction

command! -nargs=* -range VimSpeak <line1>,<line2>call VimSpeak(<count>, <q-args>)

" vim:set ts=8 sts=2 sw=2 tw=0:
