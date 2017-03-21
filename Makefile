SEQ_3D_DIR = seq_3d
SEQ_SLOW_DIR = seq_slow

all:

	+$(MAKE) -C $(SEQ_3D_DIR)
	+$(MAKE) -C $(SEQ_SLOW_DIR)