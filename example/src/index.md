---
title: Welcome

layout: page
---

# Welcome to Basalt version {{ basalt-version }}!

Basalt is a lightweight static site generator and templating engine written in C++.
This is an example web-page created using it to show its capabilities.

# About Basalt

<ul>
    <basalt-for collection="about" as="page">
        <li style="font-size: 24px">
            <a href="{{ page.basalt-route }}">{{ page.title }}</a>
        </li>
    </basalt-for>
</ul>
