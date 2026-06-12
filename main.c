include <stdio.h>
include <stdlib.h>
include <math.h>

void calcul_reseau(unsigned int ip[], int cidr) {
    unsigned int masque[4];
    int bits = cidr;

    // Calcul du masque
    for (int i = 0; i < 4; i++) {
        if (bits >= 8) {
            masque[i] = 255;
            bits -= 8;
        } else {
            masque[i] = (unsigned char)(256 - pow(2, 8 - bits));
            bits = 0;
        }
    }

    printf("\nMasque: %u.%u.%u.%u\n", masque[0], masque[1], masque[2], masque[3]);

    // Adresse réseau
    unsigned int reseau[4];
    for (int i = 0; i < 4; i++) {
        reseau[i] = ip[i] & masque[i];
    }

    printf("Adresse reseau: %u.%u.%u.%u\n", reseau[0], reseau[1], reseau[2], reseau[3]);

    // Broadcast
    unsigned int broadcast[4];
    for (int i = 0; i < 4; i++) {
        broadcast[i] = reseau[i] | (~masque[i] & 255);
    }

    printf("Broadcast: %u.%u.%u.%u\n", broadcast[0], broadcast[1], broadcast[2], broadcast[3]);

    // Nombre d'hôtes
    int nb_hotes = pow(2, (32 - cidr)) - 2;
    printf("Nombre d'hotes: %d\n", nb_hotes);
}

int main() {
    unsigned int ip[4];
    int cidr;

    printf("Entrez une IP (format x.x.x.x): ");
    scanf("%u.%u.%u.%u", &ip[0], &ip[1], &ip[2], &ip[3]);

    printf("Entrez le CIDR (ex: 24): ");
    scanf("%d", &cidr);

    calcul_reseau(ip, cidr);

    return 0;
}

