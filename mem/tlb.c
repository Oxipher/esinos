#include <stdio.h>
#include "mem.h"

double mem_eat(double hit_rate) {
    double hit_cost = TLB_LOOKUP_NS + MEM_ACCESS_NS;
    double miss_cost = TLB_LOOKUP_NS + 2 * MEM_ACCESS_NS;

    return hit_rate * hit_cost + (1.0 - hit_rate) * miss_cost;
}

void mem_tlb_report(const int *refs, int n_refs) {
    int tlb[TLB_SIZE];

    for (int e = 0; e < TLB_SIZE; e++) {
        tlb[e] = -1;
    }

    int hits = 0;

    for (int i = 0; i < n_refs; i++) {
        int p = refs[i];
        int pos = -1;

        for (int e = 0; e < TLB_SIZE; e++) {
            if (tlb[e] == p) {
                pos = e;
                break;
            }
        }

        if (pos >= 0) {
            hits++;
        }

        int start = (pos >= 0) ? pos : TLB_SIZE - 1;

        for (int e = start; e > 0; e--) {
            tlb[e] = tlb[e - 1];
        }

        tlb[0] = p;
    }

    double h = (double)hits / (double)n_refs;

    printf("TLB de %d entrees sur %d refs : %d hits, h = %.1f %%\n",
           TLB_SIZE, n_refs, hits, 100.0 * h);

    printf("sans TLB du tout : EAT = %.0f ns (2 acces memoire)\n",
           2.0 * MEM_ACCESS_NS);

    printf("h =   0 %% -> EAT = %.0f ns (TLB presente mais jamais hit)\n",
           mem_eat(0.0));

    double rates[3] = {0.80, 0.90, 1.0};

    for (int r = 0; r < 3; r++) {
        printf("h = %3.0f %% -> EAT = %.0f ns\n",
               100.0 * rates[r], mem_eat(rates[r]));
    }

    printf("courbe complete pour la figure du rapport :\n");

    for (int hc = 0; hc <= 100; hc += 10) {
        printf("  h = %3d %% -> %.0f ns\n",
               hc, mem_eat(hc / 100.0));
    }
}

