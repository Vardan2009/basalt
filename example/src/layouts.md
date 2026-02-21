---
title: Layouts

layout: page
collection: about
order: 2
---

# Layouts

**Layouts** in Basalt are reusable HTML files that, combined with [Page Data](/pages), create a singular page

## Example Layout

```
<html xmlns="http://www.w3.org/1999/xhtml">
    <head>
        <title>{{ website-name }} - {{ title }}</title>

        <link rel="stylesheet" href="/style.css" />
        <meta name="viewport" content="width=device-width, initial-scale=1.0" />
    </head>
    <body>
        <basalt-partial>header</basalt-partial>

        <div id="content-wrapper">
            <basalt-content />
        </div>
    </body>
</html>
```
