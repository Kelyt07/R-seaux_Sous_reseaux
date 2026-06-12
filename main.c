#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
 
/* Convertit une adresse IP sous forme de chaîne en entier 32 bits */
unsigned int ip_vers_entier(int a, int b, int c, int d) {
    return ((unsigned int)a << 24) | ((unsigned int)b << 16) | ((unsigned int)c << 8) | (unsigned int)d;
}
 
/* Affiche une adresse IP depuis un entier 32 bits */
void afficher_ip(unsigned int ip) {
    printf("%u.%u.%u.%u", (ip >> 24) & 0xFF, (ip >> 16) & 0xFF, (ip >> 8) & 0xFF, ip & 0xFF);
}
 
/* Retourne le masque en entier à partir du préfixe CIDR */
unsigned int masque_depuis_cidr(int cidr) {
    if (cidr == 0) return 0;
    return 0xFFFFFFFF << (32 - cidr);
}
 
/* Détermine la classe de l'adresse IP */
char classe_ip(unsigned int ip) {
    unsigned char premier_octet = (ip >> 24) & 0xFF;
    if (premier_octet < 128)       return 'A';
    else if (premier_octet < 192)  return 'B';
    else if (premier_octet < 224)  return 'C';
    else                           return '?';
}
 
/* Retourne le masque par défaut selon la classe */
int cidr_defaut(unsigned int ip) {
    char c = classe_ip(ip);
    if (c == 'A') return 8;
    if (c == 'B') return 16;
    if (c == 'C') return 24;
    return 24;
}
 
void Reseaux_Sous_reseaux(int a, int b, int c, int d, int msr) {
    unsigned int ip       = ip_vers_entier(a, b, c, d);
    int          cidr_nat = cidr_defaut(ip);
 
    /* Masque de sous-réseau saisi */
    unsigned int masque   = masque_depuis_cidr(msr);
 
    /* Bits empruntés pour le sous-réseau */
    int bits_empruntes    = msr - cidr_nat;
    int bits_hotes        = 32 - msr;
 
    /* Capacités */
    long nb_sous_reseaux  = (bits_empruntes >= 0) ? (long)pow(2, bits_empruntes) : 1;
    long nb_hotes         = (bits_hotes >= 2)     ? (long)pow(2, bits_hotes) - 2 : 0;
 
    /* Adresse réseau principal */
    unsigned int reseau_principal = ip & masque_depuis_cidr(cidr_nat);
 
    printf("\n========================================\n");
    printf("   RESEAUX_SOUS_RESEAUX - Résultats\n");
    printf("========================================\n\n");
 
    printf("Adresse IP saisie   : %d.%d.%d.%d\n", a, b, c, d);
    printf("Classe              : %c  (masque naturel /%d)\n", classe_ip(ip), cidr_nat);
    printf("MSR (CIDR)          : /%d\n", msr);
    printf("Masque de s-réseau  : ");
    afficher_ip(masque);
    printf("\n\n");
 
    printf("--- Capacités ----------------------------------------\n");
    printf("Capacité de réseaux        : 1  (réseau de classe %c)\n", classe_ip(ip));
    printf("Capacité de sous-réseaux   : %ld\n", nb_sous_reseaux);
    printf("Adresses par sous-réseau   : %ld  (dont %ld utilisables)\n",
           (long)pow(2, bits_hotes), nb_hotes);
    printf("------------------------------------------------------\n\n");
 
    printf("--- Sous-réseaux -------------------------------------\n");
    printf("%-5s %-18s %-18s %-18s\n",
           "N°", "Adresse réseau", "Première hôte", "Broadcast");
    printf("%-5s %-18s %-18s %-18s\n",
           "---", "--------------", "-------------", "---------");
 
    unsigned int taille_bloc = (unsigned int)pow(2, bits_hotes);
 
    for (long i = 0; i < nb_sous_reseaux; i++) {
        unsigned int adr_reseau    = reseau_principal + (unsigned int)(i * taille_bloc);
        unsigned int adr_broadcast = adr_reseau + taille_bloc - 1;
        unsigned int premier_hote  = adr_reseau + 1;
 
        char buf_r[20], buf_b[20], buf_h[20];
 
        sprintf(buf_r, "%u.%u.%u.%u",
                (adr_reseau >> 24) & 0xFF, (adr_reseau >> 16) & 0xFF,
                (adr_reseau >> 8) & 0xFF,   adr_reseau & 0xFF);
 
        sprintf(buf_b, "%u.%u.%u.%u",
                (adr_broadcast >> 24) & 0xFF, (adr_broadcast >> 16) & 0xFF,
                (adr_broadcast >> 8) & 0xFF,   adr_broadcast & 0xFF);
 
        sprintf(buf_h, "%u.%u.%u.%u",
                (premier_hote >> 24) & 0xFF, (premier_hote >> 16) & 0xFF,
                (premier_hote >> 8) & 0xFF,   premier_hote & 0xFF);
 
        printf("%-5ld %-18s %-18s %-18s\n", i + 1, buf_r, buf_h, buf_b);
    }
 
    printf("------------------------------------------------------\n");
}
 
int main(void) {
    int a, b, c, d, msr;
 
    printf("=== Reseaux_Sous_reseaux ===\n\n");
    printf("Entrez l'adresse IPv4 (format: A B C D) : ");
    if (scanf("%d %d %d %d", &a, &b, &c, &d) != 4) {
        fprintf(stderr, "Erreur de saisie de l'adresse IP.\n");
        return EXIT_FAILURE;
    }
 
    /* Validation basique */
    if (a < 0 || a > 223 || b < 0 || b > 255 ||
        c < 0 || c > 255 || d < 0 || d > 255) {
        fprintf(stderr, "Adresse IP invalide (classes A/B/C uniquement).\n");
        return EXIT_FAILURE;
    }
 
    printf("Entrez le MSR (longueur du masque CIDR, ex: 26) : /");
    if (scanf("%d", &msr) != 1 || msr < 1 || msr > 30) {
        fprintf(stderr, "MSR invalide (doit être entre 1 et 30).\n");
        return EXIT_FAILURE;
    }
 
    /* Vérification cohérence classe / MSR */
    unsigned int ip       = ip_vers_entier(a, b, c, d);
    int          cidr_nat = cidr_defaut(ip);
    if (msr < cidr_nat) {
        fprintf(stderr, "Le MSR /%d est inférieur au masque naturel /%d de la classe %c.\n",
                msr, cidr_nat, classe_ip(ip));
        return EXIT_FAILURE;
    }
 
    Reseaux_Sous_reseaux(a, b, c, d, msr);
 
    return EXIT_SUCCESS;
}
 

