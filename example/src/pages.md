---
title: Pages

layout: page
collection: about
---

# Pages

**Pages** in Basalt use Markdown (+ optional HTML) for content and a YAML Frontmatter for required and custom metadata

## Example Page
```
---
title: My Webpage

layout: page        # determines what layout is used
collection: about   # (optional) what collection the page is in (used in &lt;basalt-for&gt;)
---

# You can write Markdown

&lt;h1&gt;...or HTML here!&lt;/h1&gt;
```
