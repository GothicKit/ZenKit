#  Copyright © 2024 GothicKit Contributors.
#  SPDX-License-Identifier: MIT
import os
import re
import sys
import yaml
from typing import Any

from jinja2 import Template

LINK_BASIC = re.compile(r"\[(\w+)\]")
LINK_FIELD = re.compile(r"\[(\w+):(\w+)\]")


def _resolve_links_str(str: str) -> str:
    text = re.sub(LINK_BASIC, r"[`\1`](\1.md)", str)
    text = re.sub(LINK_FIELD, lambda g: f"[`{g.group(1)}:{g.group(2)}`]({g.group(1)}.md#{g.group(2).lower()})", text)
    return text


def _resolve_links(cfg: dict[str, Any]):
    if "class" in cfg:
        cfg["class"]["description"] = _resolve_links_str(cfg["class"]["description"])

    if "properties" not in cfg:
        return

    for prop in cfg["properties"].values():
        if prop["type"] == "group":
            _resolve_links(prop)
        elif "description" in prop:
            prop["description"] = _resolve_links_str(prop["description"])


def main(paths: list[str]) -> None:
    with open("_template.mdt", "r") as fp:
        env = Template(fp.read(), trim_blocks=True, lstrip_blocks=True)

    for path in paths:
        print(path)
        with open(path, "rb") as fp:
            cfg = yaml.safe_load(fp)

        _resolve_links(cfg)

        with open(cfg["class"]["name"] + ".md", "w") as fp:
            fp.write(re.sub(r"\n{2,}", "\n\n", env.render(cfg)).lstrip())


if __name__ == "__main__":
    if len(sys.argv) < 2:
        paths = [i for i in os.listdir(".") if i .endswith(".yml")]
        main(paths)
    else:
        main(sys.argv[1:])
