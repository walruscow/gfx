blue = gr.material({0.7, 0.6, 1}, {0.5, 0.4, 0.8}, 25)

root = gr.node('root')

-- A cube
p1 = gr.mesh('p1',
{{-2, -2, -2},
 {-2, 0, -2},
 {0, 0, -2},
 {0, -2, -2},
 {-2, -2, 0},
 {-2, 0, 0},
 {0, 0, 0},
 {0, -2, 0},
 },
 {{0, 3, 2, 1}})
root:add_child(p1)
p1:set_material(blue)

l1 = gr.light({0, 0, 3}, {0.8, 0.8, 0.8}, {1, 0, 0})

gr.render(root, 'sample.png', 256, 256,
	  {0, 0, 3,}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.4, 0.4, 0.4}, {l1})
