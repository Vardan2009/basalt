# Basalt Static Site Generator

Each page is a markdown file with a YAML front matter.

Project config with YAML (`basalt.yml`)
```yml
site:
    # root directory for markdown files
    pages: src
    # root directory for templates
    layouts: layouts
    # root directory for common stuff that would be included in other pages
    partials: partials
    # public dir (images and stuff)
    public: public
    
    # global data
    data:
        website-name: Basalt Example Website

    # output directory
    out: dist
```

markdown page
```md
---
key: value
date: 202

# This one is special, it specifies in which collection the
# page is in, which can be used in <basalt-for>
collection: blog
---

# This is content!!!
```

html usage

```html
<basalt-partial>header</basalt-partial>
<!-- {{}} works even in tags -->
<title>{{ global.website-name }}</title>
<p>{{ page.key }}</p>
<basalt-content />

<basalt-for collection="blog">
    <li>
        <a href="{{ url }}">{{ title }} @ {{ date }}</a>
    </li>
</basalt-for>
```

Routing
Each page in `src` will generate a new route.

So this
```
test/a/b/c.md
```
will generate to (in dist)
```
/test/a/b/c/index.html
```

`index.md` is special

```
test/a/b/index.md
```
will generate to
```
/test/a/b
```
