---
title: Main Page

layout: page
collection: webpages
---

# Hello!
## This is a subtitle

# The name of this website is {{ website-name }}

This is **basalt**!

## Sitemap

<ul>
<basalt-for collection="webpages" as="page">
	<li>
		<a href="{{ page.basalt-route }}">{{ page.title }}</a>
	</li>
</basalt-for>
</ul>

# This shouldn't work ==> {{ page.basalt-route }}