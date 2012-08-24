%!
%%
%% This is file `xcolor.pro',
%% generated with the docstrip utility.
%%
%% The original source files were:
%%
%% xcolor.dtx  (with options: `dvips')
%% 
%% IMPORTANT NOTICE:
%% 
%% For the copyright see the source file.
%% 
%% Any modified versions of this file must be renamed
%% with new filenames distinct from xcolor.pro.
%% 
%% For distribution of the original source see the terms
%% for copying and modification in the file xcolor.dtx.
%% 
%% This generated file may be distributed as long as the
%% original source files, as listed above, are part of the
%% same distribution. (The sources need not necessarily be
%% in the same archive or directory.)
%%
%% xcolor.pro (PostScript/dvips header file)
%%  [2005/12/21 v2.09 LaTeX color extensions (UK)]
%%
%% ----------------------------------------------------------------
%% Copyright (C) 2003-2005 by Dr. Uwe Kern <xcolor at ukern dot de>
%% ----------------------------------------------------------------
%%
userdict begin
/setcmycolor{0 setcmykcolor}def
/XCbd{bind def}bind def
/XCed{exch def}XCbd
/XCdef{exch dup TeXDict exch known{pop pop}{XCed}ifelse}XCbd
/XCsetcolor{cvx counttomark array astore cvx exch pop XCdef}XCbd
/XCcmy{mark exch exec/setcmycolor XCsetcolor}XCbd
/XCcmyk{mark exch exec/setcmykcolor XCsetcolor}XCbd
/XCgray{mark exch exec/setgray XCsetcolor}XCbd
/XChsb{mark exch exec/sethsbcolor XCsetcolor}XCbd
/XCrgb{mark exch exec/setrgbcolor XCsetcolor}XCbd
/XC!b{save/setgray{pop}def}XCbd
/XC!e{restore}XCbd
/XCsp2bp{1 65781.76 div dup scale}XCbd
/XCsetline
 {setlinewidth [] 0 setdash 2 setlinecap 0 setlinejoin 4 setmiterlimit}XCbd
/XCboxframe{/d XCed/h XCed/w XCed/l XCed w 0 gt{h d neg gt{l 0 gt
 {XCsp2bp l XCsetline
  l 2 div dup translate
  0 d neg w l sub h l sub d add rectstroke}if}if}if}XCbd
/XCcolorblock{/d XCed/h XCed/w XCed w 0 gt{h d neg gt
 {XCsp2bp 0 XCsetline
  0 d neg w h d add rectfill}if}if}XCbd
end
%% End of file `xcolor.pro'.
