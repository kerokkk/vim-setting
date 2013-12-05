" syntax file for TINA log files
" Maintainer:	Keizaburo Takase
" Last Change:	2013/12/05

syn keyword ERROR       Error
syn keyword INFORMATION Info[rmation]
syn keyword WARNING     Warning
syn keyword TRACE       Programtrace

hi  def link    ERROR           ErrorMsg
hi  def link    INFORMATION     ColorColumn
hi  def link    WARNING         Search
hi  def link    TRACE           Type
