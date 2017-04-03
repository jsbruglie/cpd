# Makefile for all the main versions
# Assumes every version in its directory

# Directories
SEQ_3D_DIR = seq_3d_matrix_swap
SEQ_GRID = seq_grid 
SEQ_GRID_LIST = seq_grid_list
SEQ_GRID_HASH = seq_grid_hash
PAR_GRID = par_grid
PAR_GRID_LIST = par_grid_list 
PAR_GRID_HASH = par_grid_hash 

all:

	+$(MAKE) -C $(SEQ_3D_DIR)
	+$(MAKE) -C $(SEQ_GRID)
	+$(MAKE) -C $(SEQ_GRID_LIST)
	+$(MAKE) -C $(SEQ_GRID_HASH)
	+$(MAKE) -C $(PAR_GRID)
	+$(MAKE) -C $(PAR_GRID_LIST)
	+$(MAKE) -C $(PAR_GRID_HASH)

clean:

	+$(MAKE) -C $(SEQ_3D_DIR) clean
	+$(MAKE) -C $(SEQ_GRID) clean
	+$(MAKE) -C $(SEQ_GRID_LIST) clean
	+$(MAKE) -C $(SEQ_GRID_HASH) clean
	+$(MAKE) -C $(PAR_GRID) clean
	+$(MAKE) -C $(PAR_GRID_LIST) clean
	+$(MAKE) -C $(PAR_GRID_HASH) clean
