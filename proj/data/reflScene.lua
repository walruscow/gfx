require('boxscene')

grey = gr.material({0.2, 0.2, 0.2}, {0, 0, 0}, 0)
yellow = gr.material({0.75, 0.75, 0}, {0, 0, 0}, 15)
mirror = gr.material({.1, .1, .1}, {0.6, 0.6, 0.6}, 15)
bw = gr.function_material("bw_squares", {0, 0, 0}, 25)

sphere = gr.sphere('sphere')
sphere:set_material(mirror)
sphere:translate(0, -roomSize / 2 + 2, -4)
scene:add_child(sphere)

--floor:set_material(mirror)
roof:set_material(grey)

SIZE = 128
gr.render(scene, 'reflScene.png', SIZE, SIZE,
    camera, cameraDirection, {0, 1, 0}, 50,
    {0.4, 0.4, 0.4}, {topLight, cameraLight})
