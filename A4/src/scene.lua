red = gr.material({1, 0, 0}, {0, 0, 0}, 0)

root = gr.node('root')

s1 = gr.sphere('s1', {0, 0, 0}, 1)
root:add_child(s1)
s1:set_material(red)

whiteLight = gr.light({-10, -10, -10}, {1, 1, 1}, {1, 1, 1})

gr.render(root, 'ez.png', 10, 10, -- size and name
          {0, 0, 0}, {1, 0 ,0}, -- view position, direction
          {0, 1, 0}, 45, -- up and fov
          {0.3, 0.3, 0.3}, {whiteLight}) -- lighting
