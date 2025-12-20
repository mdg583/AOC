#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <regex.h>
#include <unistd.h>
#include <termios.h>

static int tty_fd;
static struct termios tty_old;

void tty_init(void) {
    struct termios t;
    FILE *tty = fopen("/dev/tty", "r");
    tty_fd = fileno(tty);
    tcgetattr(tty_fd, &tty_old);
    t = tty_old;
    t.c_lflag &= ~(ICANON | ECHO);
    t.c_cc[VMIN] = 0;
    t.c_cc[VTIME] = 0;
    tcsetattr(tty_fd, TCSANOW, &t);
}

int tty_getch(void) {
    unsigned char c;
    return read(tty_fd, &c, 1) == 1 ? c : -1;
}

void tty_done(void) {
    tcsetattr(tty_fd, TCSANOW, &tty_old);
}

int gmap_up(int gmap, int n){
    if(n==1) return (gmap >> 3) & 0b0000000000111111;
    if(n==2) return (gmap >> 6) & 0b0000000000000111;
}

int gmap_down(int gmap, int n){
    if(n==1) return (gmap << 3) & 0b0000000111111000;
    if(n==2) return (gmap << 6) & 0b0000000111000000;
}

int gmap_left(int gmap, int n){
    if(n==1) return (gmap >> 1) & 0b0000000011011011;
    if(n==2) return (gmap >> 2) & 0b0000000001001001;
}

int gmap_right(int gmap, int n){
    if(n==1) return (gmap << 1) & 0b0000000110110110;
    if(n==2) return (gmap << 2) & 0b0000000100100100;
}

int gmap_flip(int gmap){
    int lcol = gmap_right(gmap, 2);
    int rcol = gmap_left(gmap, 2);
    int mcol = gmap & 0b0000000010010010;
    return lcol | rcol | mcol;
}

int gmap_rot1(int gmap){
    int r,b;
    r = 0;
    b = gmap & 1;
    // upper left to upper right
    r |= (1 << 2) * b; gmap = gmap>>1; b = gmap & 1;
    // center top to center right
    r |= (1 << 5) * b; gmap = gmap>>1; b = gmap & 1;
    // upper right to lower right
    r |= (1 << 8) * b; gmap = gmap>>1; b = gmap & 1;
    // center left to center top
    r |= (1 << 1) * b; gmap = gmap>>1; b = gmap & 1;
    // center to center
    r |= (1 << 4) * b; gmap = gmap>>1; b = gmap & 1;
    // center right to center bottom
    r |= (1 << 7) * b; gmap = gmap>>1; b = gmap & 1;
    // lower left to upper left
    r |= (1 << 0) * b; gmap = gmap>>1; b = gmap & 1;
    // center bottom to center left
    r |= (1 << 3) * b; gmap = gmap>>1; b = gmap & 1;
    // lower right to lower left
    r |= (1 << 6) * b;
    return r;
}

int gmap_rot(int gmap, int n){
    n = n % 4;
    for(int i = 0; i < n; i++) gmap = gmap_rot1(gmap);
    return gmap;
}

int count_bits(int i){
    int cnt = 0;
    while(i > 0){
        cnt += i & 1;
        i /= 2;
    }
    return cnt;
}

typedef struct {
    int id;
    int gmap;
    int x; // center
    int y; // center
    int color; // 0 - 5
} Gift;

// How many pixels overlap for two gifts?
int gift_overlap(Gift g1, Gift g2){
    if(abs(g1.x - g2.x) > 2 || abs(g1.y - g2.y) > 2) return 0;
    int m1 = g1.gmap;
    int m2 = g2.gmap;
    if(g1.x < g2.x) m2 = gmap_left(m2, g2.x - g1.x);
    if(g1.x > g2.x) m2 = gmap_right(m2, g1.x - g2.x);
    if(g1.y < g2.y) m2 = gmap_up(m2, g2.y - g1.y);
    if(g1.y > g2.y) m2 = gmap_down(m2, g1.y - g2.y);
    // count the overlap bits
    int overlap = m1 & m2;
    return count_bits(overlap);
}

// Count overlaps of this gift against all others
int cnt_gift_overlaps(Gift this, Gift *gifts, int n){
    int overlaps = 0;
    for(int i = 0; i < n; i++){
        if(gifts[i].id == this.id) continue;
        overlaps += gift_overlap(this, gifts[i]);
    }
    return overlaps;
}

int cnt_world_overlap(Gift this, int w, int h){
    if(this.x < -1 || this.x > w || this.y < -1 || this.y > h){
        int overlap = 0;
        if(this.x < -1) overlap += -1 - this.x;
        if(this.y < -1) overlap += -1 - this.y;
        if(this.x > w) overlap += this.x - w;
        if(this.y > h) overlap += this.y - h;
        return count_bits(this.gmap) + overlap;
    }
    int world_map = 0;
    if(this.x == 0)    world_map |= 0b0000000001001001;
    if(this.x == -1)   world_map |= 0b0000000011011011;
    if(this.x == w-1)  world_map |= 0b0000000100100100;
    if(this.x == w)    world_map |= 0b0000000110110110;
    if(this.y == 0)    world_map |= 0b0000000000000111;
    if(this.y == -1)   world_map |= 0b0000000000111111;
    if(this.y == h-1)  world_map |= 0b0000000111000000;
    if(this.y == h)    world_map |= 0b0000000111111000;
    int overlap = this.gmap & world_map;
    return count_bits(overlap);
}

typedef struct {
    int w;
    int h;
    int gmaps[6];
    int gift_count[6];
} Simulation;

void gotoxy(int x, int y) {
    printf("\033[%d;%dH", y, x);
}

// colors 31 - 36
void change_color(int n){
    n = n % 6;
    printf("\x1B[%dm", 31 + n);
}

void reset_color(){
    printf("\x1B[0m");
}

void render_gift(Gift g, int x0, int y0){
    int gmap = g.gmap;
    change_color(g.color);
    for(int i = 0; i < 9; i++){
        int x = (i % 3) - 1;
        int y = (i / 3) - 1;
        int nx = x0 + g.x + x;
        int ny = y0 + g.y + y;
        if(gmap & 1) {
            if(nx > 0 && nx < 120 && ny > 0 && ny < 80){
                gotoxy(nx,ny);
                printf("#");
            }
        }
        gmap >>= 1;
    }
    reset_color();
}

void display_simulation(Simulation s, Gift *gifts, int n, char *bottom_line){
    // screen is 120 x 25
    // rander border
    printf("\033[2J");
    gotoxy(5,5-1);
    for(int x = 0; x < s.w; x++){
        printf("-");
    }
    gotoxy(5,5+s.h);
    for(int x = 0; x < s.w; x++){
        printf("-");
    }
    for(int y = 0; y < s.h; y++){
        gotoxy(5-1,5+y);
        printf("|");
        gotoxy(5+s.w,5+y);
        printf("|");
    }
    for(int i = 0; i < n; i++){
        render_gift(gifts[i], 5, 5);
    }
    gotoxy(0,s.h+5+2);
    printf("%s\n", bottom_line);
}

void print_gift(Gift g){
    printf("x: %d, y: %d, color: %d, gmap: %d\n",g.x,g.y,g.color,g.gmap);
}

int simulate(Simulation s, char* name){
    char bottom[80];
    int w = s.w;
    int h = s.h;
    int total_gifts = 0;
    for(int i = 0; i < 6; i++){
        total_gifts += s.gift_count[i];
    }

    printf("Gifts: %d\n", total_gifts);

    Gift *gifts = (Gift *)malloc(sizeof(Gift)*total_gifts);
    int k = 0;
    for(int i = 0; i < 6; i++){
        for(int j = 0; j < s.gift_count[i]; j++){
            Gift g;
            g.gmap = s.gmaps[i];
            // randomize flip
            if(rand() % 2) g.gmap = gmap_flip(g.gmap);
            // randomize rotation
            g.gmap = gmap_rot(g.gmap, rand() % 4);
            // randomize location
            g.x = (rand() % (w+2)) - 1;
            g.y = (rand() % (h+2)) - 1;
            g.color = i % 6;
            g.id = k;
            gifts[k] = g;
            k++;
        }
    }

    // make a prediction
    int fit_gifts = (w/3)*(h/3);
    char pred_flag = (fit_gifts >= total_gifts) ? '*' : ' ';

    display_simulation(s, gifts, total_gifts, "Simulated!");
    int caution = 124;
    int max_sims = 10000;
    int shortcut_overlap = 200;
    int shortcut_time = 1000;
    int render_period = 500;
    int sleep_time = 0;
    int i;
    int total_overlap;
    tty_init();
    for(i = 0; i < max_sims; i++){
        caution = i/2;
        for(int j = 0; j < total_gifts; j++){
            Gift g1 = gifts[j];
            Gift g2 = g1;
            int action = rand() % 6;
            if(action < 4){
                g2.x += (rand() % 3) - 1;
                g2.y += (rand() % 3) - 1;
            }
            if(action == 4){
                g2.gmap = gmap_flip(g2.gmap);
            }
            if(action == 5){
                g2.gmap = gmap_rot(g2.gmap, rand() % 4);
            }
            int o1 =
                cnt_gift_overlaps(g1,gifts,total_gifts) +
                cnt_world_overlap(g1,w,h);
            int o2 =
                cnt_gift_overlaps(g2,gifts,total_gifts) +
                cnt_world_overlap(g2,w,h);
            int pr = caution*(o2-o1);
            if(pr <= 0 || (rand() % pr == 0) ){
                gifts[j] = g2;
            }
        }
        total_overlap = 0;
        for(int j = 0; j < total_gifts; j++){
            total_overlap +=
                cnt_gift_overlaps(gifts[j],gifts,total_gifts) +
                cnt_world_overlap(gifts[j],w,h);
        }
        if(i > shortcut_time && total_overlap > shortcut_overlap) break;
        if((i % render_period) == 0){
            int key = tty_getch();
            snprintf(bottom,80,"%s%c [%4d] Overlap=%3d cuation=%2d key=%2d\n", name, pred_flag, i, total_overlap, caution,key);
            display_simulation(s, gifts, total_gifts, bottom);
            usleep(sleep_time);
            if(key == 10){
                break; // next simulation
            }
            if(key == 61){
                caution <<= 1;
            }
            if(key == 45){
                caution >>= 1;
            }
        }
        if(total_overlap == 0) break;
    }

    //int total_bits = 0;
    //for(int j = 0; j < total_gifts; j++) total_bits += count_bits(gifts[j].gmap);

    free(gifts);
    tty_done();
    //if(total_overlap == 0) return 0;
    //printf("Cheat! Good=%d, Bad=%d\n", (fit_gifts >= total_gifts), total_bits > (w*h));
    //if((fit_gifts >= total_gifts)) usleep(3000000);
    //if(fit_gifts >= total_gifts) return 0;
    //if(total_bits > (w*h)) return 10000;
    return total_overlap;
}

int match_gmap_start(char *line){
    regex_t regex; int match;
    regcomp(&regex, "^[0-9]+:", REG_EXTENDED| REG_NOSUB);
    match = (regexec(&regex, line, 0, NULL, 0) == 0);
    regfree(&regex);
    return match;
}

int match_regex(char *line, char *pattern){
    regex_t regex; int match;
    regcomp(&regex, pattern, REG_EXTENDED| REG_NOSUB);
    match = (regexec(&regex, line, 0, NULL, 0) == 0);
    regfree(&regex);
    return match;
}

int gmap_line(char *line){
    int r = 0;
    r |= 1*(line[0]=='#');
    r |= 2*(line[1]=='#');
    r |= 4*(line[2]=='#');
    return r;
}

int read_inputs(Simulation *sims, int max_sims){
    int gmaps[6];
    char line[1024];
    int adding_gift = 0;
    int i = -1; // current gift
    int j = 0; // current simulation
    int gift_row = 0;
    int gmap = 0;
    while(fgets(line, 1024, stdin) != NULL){
        if(match_regex(line, "^[0-9]+:")){
            i++;
            gift_row = 0;
            continue;
        }
        if(match_regex(line, "^[#\\.]{3}")){
            if(gift_row > 2) continue;
            if(i < 0 || i > 5) continue;
            int gline = gmap_line(line);
            gmaps[i] |= (gline << (3*gift_row));
            gift_row++;
            continue;
        }
        if(match_regex(line, "([0-9]+)x([0-9]+): ([0-9 ]*[0-9])")){
            // generate a simulation
            if(j >= max_sims) continue;
            Simulation s;
            char *token = strtok(line, "x: ");
            s.w = atoi(token);
            token = strtok(NULL, "x: ");
            s.h = atoi(token);
            token = strtok(NULL, "x: ");
            int gift_i = 0;
            while(token != NULL){
                if(gift_i < 6){
                    s.gift_count[gift_i] = atoi(token);
                    gift_i++;
                }
                token = strtok(NULL, "x: ");
            }
            for(int i = 0; i < 6; i++) s.gmaps[i] = gmaps[i];
            sims[j] = s;
            j++;
        }
    }
    return j;
}

int main(){
    char name[32];
    srand(time(NULL));
    Simulation sims[1024];
    int solved[1024];
    int num_sims = read_inputs(sims, 1024);
    for(int i = 0; i < num_sims; i++){
        snprintf(name,32,"Sim %d", i);
        for(int k = 0; k < 3; k++){
            solved[i] = simulate(sims[i], name);
            if(solved[i] == 0 || solved[i] > 200) break;
        }
    }
    int total_fit = 0;
    for(int i = 0; i < num_sims; i++){
        printf("Simulation: w=%d, h=%d: %s in %d\n", sims[i].w, sims[i].h, solved[i] ? "FAILED": "SOLVED",solved[i]);
        total_fit += solved[i]==0;
    }
    printf("Total fit: %d\n", total_fit);
}
