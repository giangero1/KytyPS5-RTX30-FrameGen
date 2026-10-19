"""Validate actual draw/resource captures: UI remains 4K, scene stays 1440p.

Usage: python tests/VerifyUiCanvasCapture.py <frame.jsonl> [golden-frame.jsonl]
Captures contain locally recorded metadata; no game files are distributed here.
"""
import json
import sys

UI_SHADERS = {"7e880a66bf1b69c6", "b2f062027869d4ee", "bd39a2eba2d20d8b"}


def inspect(path):
    with open(path, encoding="utf-8") as source:
        calls = [json.loads(line) for line in source]
    ui = []
    composites = []
    for call in calls:
        targets = [i for i in call.get("img", []) if i["k"] == 0]
        sampled = [i for i in call.get("img", []) if i["k"] == 2]
        if call.get("vs") in UI_SHADERS:
            ui.extend((call, image) for image in targets if image["w"] >= 2560 and image["h"] >= 1440)
        for target in targets:
            if (target["w"], target["h"]) == (3840, 2160):
                ui_inputs = [i for i in sampled if (i["w"], i["h"]) == (3840, 2160)]
                scene_inputs = [i for i in sampled if (i["w"], i["h"]) == (2560, 1440)]
                if ui_inputs and scene_inputs:
                    composites.append((call, ui_inputs, scene_inputs))
    assert ui, "No full-size interface draws captured"
    assert all((i["w"], i["h"]) == (3840, 2160) for _, i in ui), "Interface allocation was reduced"
    assert all(c["vp"][:4] == [1920, 1920, -1080, 1080] for c, _ in ui), "Interface viewport transformed"
    assert composites, "No full-UI plus native-scene composite captured"
    # The final composite must sample the actual attachment drawn by the interface.
    addresses = {i["a"] for _, i in ui}
    assert any(i["a"] in addresses for _, inputs, _ in composites for i in inputs), "UI attachment not consumed"
    print(f"{path}: {len(ui)} original 4K UI draws; {len(composites)} full-UI/native-scene composite(s)")


for capture in sys.argv[1:]:
    inspect(capture)
if len(sys.argv) == 1:
    raise SystemExit("Specify at least one frame.jsonl capture")
