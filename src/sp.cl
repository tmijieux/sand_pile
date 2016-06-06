
__kernel void
compute_n_step(__global unsigned *sp1,
               __global unsigned *sp2,
               unsigned n)
{
    int i = get_global_id(0);
    int j = get_global_id(1);
    const unsigned size = get_global_size(0);
    unsigned value = 0, index;
    
    index = j * size + i;
    for (int k = 0; k < n; ++k) {
        value = sp1[index];
        value -= 4 * (value > 3);
        if (i>0 && i<size-1 && j>0 && j<size-1) {
            value += sp1[index-size] > 3;
            value += sp1[index-1] > 3;
            value += sp1[index+1] > 3;
            value += sp1[index+size] > 3;
        }
        sp2[index] = value;
        barrier(CLK_GLOBAL_MEM_FENCE);
        sp1[index] = sp2[index];
        barrier(CLK_GLOBAL_MEM_FENCE);
    }
}
