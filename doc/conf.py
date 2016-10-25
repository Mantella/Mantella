# -*- coding: utf-8 -*-
# Project information
project = u'Mantella'
copyright = u'2016, Sebastian Niemann, Oskar Pusz, Daniel Kiechle, Wolf Bock von Wülfingen and Philipp Schiffmann'
author = u'Mantella team and contributors'
github_url = 'https://github.com/SebastianNiemann/Mantella'

version = u'0.61'
release = version + '.0'

pygments_style = 'sphinx'

add_function_parentheses = False

# General configuration
nitpicky = True
nitpick_ignore = [
  ('cpp:type', 'T'),
  ('cpp:type', 'T1'),
  ('cpp:type', 'T2'),
  ('cpp:type', 'N'),
  ('cpp:type', 'std'),
  ('cpp:type', 'std::size_t'),
  ('cpp:type', 'std::array'),
  ('cpp:type', 'std::vector'),
  ('cpp:type', 'std::function'),
  ('cpp:type', 'parameter'),
]

master_doc = 'mantella'
source_suffix = ['.rst']

extensions = [
  'sphinx.ext.mathjax',
  'sphinx.ext.githubpages',
]

# Options for HTML output

import sphinx_rtd_theme

html_theme = "sphinx_rtd_theme"
html_theme_path = [sphinx_rtd_theme.get_html_theme_path()]
html_theme_options = {
  'collapse_navigation': False,
  'display_version': True,
}

html_logo = '_static/images/logo_with_white_name.png'

html_static_path = ['_static']
html_context = {
  'display_github': True,
  'css_files': [
    '_static/css/custom.css',
]}

html_show_sourcelink = False