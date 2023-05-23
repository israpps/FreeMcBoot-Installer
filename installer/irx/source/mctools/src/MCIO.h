int ReadBlock(int port, int slot, unsigned short int PageSize, unsigned short int BlockSize, unsigned int block, void *buffer);
int WriteBlock(int port, int slot, unsigned short int PageSize, unsigned short int BlockSize, unsigned int block, void *buffer);
int ReadCluster(unsigned short int PageSize, unsigned short int ClusterSize, unsigned int cluster, void *buffer);
int ReadCluster_raw(int port, int slot, unsigned short int PageSize, unsigned short int ClusterSize, unsigned int cluster, void *buffer);
int WriteCluster(unsigned short int PageSize, unsigned short int ClusterSize, unsigned int cluster, void *buffer);
