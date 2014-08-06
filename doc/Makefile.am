# $Id: Makefile.am 1467 2008-10-06 10:18:17Z lukasz $
#
# FedStage DRMAA for LSF
# Copyright (C) 2007-2008  FedStage Systems
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

DOCUTILSFLAGS = \
 -i utf-8 \
 -o utf-8 \
 --language=en \
 --file-insertion-enabled \
 --no-generator \
 --tab-width=4
RST2HTMLFLAGS = $(DOCUTILSFLAGS) \
 --stylesheet=stylesheet.css \
 --link-stylesheet
RST2LATEXFLAGS = $(DOCUTILSFLAGS) \
 --documentclass=article \
 --documentoptions=a4paper,10pt \
 --font-encoding=OT4 \
 --graphicx-option=pdftex \
 --use-latex-footnotes \
 --footnote-references=superscript \
 --use-latex-citations \
 --use-latex-toc

readme.html: $(top_srcdir)/README
	$(RST2HTML) $(RST2HTMLFLAGS) $< $@
readme.tex: $(top_srcdir)/README
	$(RST2LATEX) $(RST2LATEXFLAGS) $< $@
.tex.pdf:
	pdflatex $< && pdflatex $<

noinst_DATA = readme.html readme.pdf
EXTRA_DIST = readme.html readme.tex readme.pdf

clean-local:
	-rm -rf *.log *.aux *.toc *.out
MAINTAINERCLEANFILES = readme.html readme.tex readme.pdf

