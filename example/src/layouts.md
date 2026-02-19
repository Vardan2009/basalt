---
title: Layouts

layout: page
collection: about
---

# Layouts

**Layouts** in Basalt are reusable HTML files that, combined with [Page Data](/pages), create a singular page

<!-- There's an issue in the markdown parser that doesn't properly escape HTML in code blocks -->
<!-- I hope it gets fixed soon -->

## Example Layout
```
&lt;html xmlns=&quot;http://www.w3.org/1999/xhtml&quot;&gt;
    &lt;head&gt;
        &lt;title&gt;{{ website-name }} - {{ title }}&lt;/title&gt;

        &lt;link rel=&quot;stylesheet&quot; href=&quot;/style.css&quot; /&gt;
        &lt;meta name=&quot;viewport&quot; content=&quot;width=device-width, initial-scale=1.0&quot; /&gt;
    &lt;/head&gt;
    &lt;body&gt;
        &lt;basalt-partial&gt;header&lt;/basalt-partial&gt;

        &lt;div id=&quot;content-wrapper&quot;&gt;
            &lt;basalt-content /&gt;
        &lt;/div&gt;
    &lt;/body&gt;
&lt;/html&gt;
```
