SEQ_3D_DIR = seq_3d_matrix_swap
SEQ_SLOW_DIR = seq_sets

all:

	+$(MAKE) -C $(SEQ_3D_DIR)
	+$(MAKE) -C $(SEQ_SLOW_DIR)