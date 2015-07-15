--
--	Data for the icosahedron
--
icosa = gr.mesh( 'icosahedron', {
	{	 1.618034	,	 1.,	 0.},
	{	-1.618034	,	 1.,	 0.},
	{	 1.618034	,	-1.,	 0.},
	{	-1.618034	,	-1.,	 0.},
	{	 1.,	 0.,	 1.618034},
	{	 1.,	 0.,	-1.618034},
	{	-1.,	 0.,	 1.618034},
	{	-1.,	 0.,	-1.618034},
	{	 0.,	 1.618034	,	 1.},
	{	 0.,	-1.618034	,	 1.},
	{	 0.,	 1.618034	,	-1.},
	{	 0.,	-1.618034	,	-1.}
	   }, {}, {
    { {0 }, {8 }, {4}},
    { {0 }, {5 }, {10}},
    { {2 }, {4 }, {9}},
    { {2 }, {11}, {5}},
    { {1 }, {6 }, {8}},
    { {1 }, {10}, {7}},
    { {3 }, {9 }, {6}},
    { {3 }, {7 }, {11}},
    { {0 }, {10}, {8}},
    { {1 }, {8 }, {10}},
    { {2 }, {9 }, {11}},
    { {3 }, {11}, {9}},
    { {4 }, {2 }, {0}},
    { {5 }, {0 }, {2}},
    { {6 }, {1 }, {3}},
    { {7 }, {3 }, {1}},
    { {8 }, {6 }, {4}},
    { {9 }, {4 }, {6}},
    { {10}, {5 }, {7}},
    { {11}, {7 }, {5}}
} )
