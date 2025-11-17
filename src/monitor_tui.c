/**
 * monitor_tui.c - Interface TUI com ncurses funcional
 * Menu interativo simples e responsivo
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <ncurses.h>
#include "../include/resource_profiler.h"
#include "../include/namespace.h"
#include "../include/cgroup.h"
#include "../include/experiments.h"

/* Cores */
#define COLOR_TITLE 1
#define COLOR_MENU 2
#define COLOR_SELECTED 3
#define COLOR_INFO 4

/* Inicializar ncurses */
void init_ncurses() {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    
    if (has_colors()) {
        start_color();
        init_pair(COLOR_TITLE, COLOR_CYAN, COLOR_BLACK);
        init_pair(COLOR_MENU, COLOR_WHITE, COLOR_BLACK);
        init_pair(COLOR_SELECTED, COLOR_BLACK, COLOR_CYAN);
        init_pair(COLOR_INFO, COLOR_GREEN, COLOR_BLACK);
    }
}

/* Finalizar ncurses */
void end_ncurses() {
    endwin();
}

/* Menu de monitoramento de recursos */
void show_monitor_menu() {
    init_ncurses();
    clear();
    attron(COLOR_PAIR(COLOR_TITLE) | A_BOLD);
    mvprintw(2, 2, "[*] Resource Monitor - Monitoramento em Tempo Real");
    attroff(COLOR_PAIR(COLOR_TITLE) | A_BOLD);
    
    mvprintw(5, 2, "Insira o PID do processo a monitorar:");
    mvprintw(6, 2, "> ");
    refresh();
    
    echo();
    curs_set(1);
    char pid_str[16];
    getstr(pid_str);
    curs_set(0);
    noecho();
    
    int pid = atoi(pid_str);
    if (pid <= 0) {
        mvprintw(8, 2, "PID inválido!");
        mvprintw(10, 2, "Pressione qualquer tecla para voltar...");
        refresh();
        getch();
        end_ncurses();
        return;
    }
    
    /* Executa o resource profiler de verdade (5 amostras x 1s) */
    mvprintw(8, 2, "Monitorando PID %d por 5 segundos...", pid);
    refresh();

    mkdir("output", 0755);
    char outpath[128];
    snprintf(outpath, sizeof(outpath), "output/monitor.csv");
    int rc = rp_run(pid, 1000, 5, outpath);
    if (rc == 0) {
        mvprintw(10, 2, "[OK] Coleta concluída");
        mvprintw(12, 2, "Dados salvos em: %s", outpath);
    } else {
        mvprintw(10, 2, "[ERRO] Falha ao coletar métricas para PID %d", pid);
        mvprintw(12, 2, "Verifique permissões e se o processo existe.");
    }
    mvprintw(18, 2, "Pressione qualquer tecla para voltar...");
    refresh();
    getch();
    end_ncurses();
}

/* Submenu real de namespaces */
static void show_namespace_menu() {
    init_ncurses();
    int sel = 0; int ch;
    const char *items[] = {
        "Listar namespaces de um PID",
        "Comparar namespaces de dois PIDs",
        "Mapear processos por tipo",
        "Overhead de criação",
        "Relatório global",
        "Voltar"
    };
    int n = 6;
    while (1) {
        clear();
        attron(COLOR_PAIR(COLOR_TITLE) | A_BOLD);
        mvprintw(1,2,"[#] Namespace Analyzer");
        attroff(COLOR_PAIR(COLOR_TITLE) | A_BOLD);
        for (int i=0;i<n;i++) {
            if (i==sel) { attron(COLOR_PAIR(COLOR_SELECTED)|A_BOLD); mvprintw(3+i,4,"> %d) %s", i+1, items[i]); attroff(COLOR_PAIR(COLOR_SELECTED)|A_BOLD);} else { mvprintw(3+i,4,"  %d) %s", i+1, items[i]); }
        }
        mvprintw(12,2,"SETINHA navega, ENTER seleciona, Q sai");
        refresh();
        ch = getch();
        if (ch=='q'||ch=='Q') break;
        if (ch==KEY_UP) sel=(sel-1+n)%n; else if (ch==KEY_DOWN) sel=(sel+1)%n; else if (ch=='\n') {
            if (sel==n-1) break;
            clear();
            if (sel==0) { /* listar */
                echo(); curs_set(1); char buf[32]; mvprintw(2,2,"PID: "); getstr(buf); noecho(); curs_set(0); pid_t p=(pid_t)atoi(buf); 
                if (p > 0) { end_ncurses(); namespace_list_for_pid(p); printf("\nPressione ENTER..."); getchar(); init_ncurses(); } else { mvprintw(4,2,"PID inválido!"); }
            } else if (sel==1) { /* comparar */
                echo(); curs_set(1); char a[32], b[32]; mvprintw(2,2,"PID1: "); getstr(a); mvprintw(3,2,"PID2: "); getstr(b); noecho(); curs_set(0); 
                pid_t p1=(pid_t)atoi(a), p2=(pid_t)atoi(b); 
                if (p1 > 0 && p2 > 0) { end_ncurses(); namespace_compare(p1,p2); printf("\nPressione ENTER..."); getchar(); init_ncurses(); } else { mvprintw(4,2,"PIDs inválidos!"); }
            } else if (sel==2) { /* mapear */
                echo(); curs_set(1); char t[32]; mvprintw(2,2,"Tipo (pid/net/mnt/uts/ipc/user/cgroup): "); getstr(t); noecho(); curs_set(0); 
                if (strlen(t) > 0) { end_ncurses(); namespace_map_by_type(t); printf("\nPressione ENTER..."); getchar(); init_ncurses(); } else { mvprintw(4,2,"Tipo inválido!"); }
            } else if (sel==3) { /* overhead */
                echo(); curs_set(1); char t[32], it[32]; mvprintw(2,2,"Tipo: "); getstr(t); mvprintw(3,2,"Iterações: "); getstr(it); noecho(); curs_set(0); 
                int iterations=atoi(it); if (iterations <= 0) iterations = 10;
                if (strlen(t) > 0) { end_ncurses(); namespace_creation_overhead(t, iterations); printf("\nPressione ENTER..."); getchar(); init_ncurses(); } else { mvprintw(4,2,"Tipo inválido!"); }
            } else if (sel==4) { end_ncurses(); namespace_system_report(); printf("\nPressione ENTER..."); getchar(); init_ncurses(); }
            mvprintw(10,2,"Pressione qualquer tecla para continuar..."); refresh(); getch();
        } else if (ch>='1'&&ch<='6') { sel=ch-'1'; }
    }
    end_ncurses();
}

/* Submenu real de cgroups */
static void show_cgroup_menu() {
    init_ncurses(); int sel=0; int ch; const char *items[]={
        "Criar cgroup",
        "Ler métricas",
        "Mover PID",
        "Set CPU quota",
        "Set Mem max",
        "Voltar"
    }; int n=6;
    while (1) {
        clear(); 
        attron(COLOR_PAIR(COLOR_TITLE)|A_BOLD); 
        mvprintw(1,2,"[+] Cgroup Manager"); 
        attroff(COLOR_PAIR(COLOR_TITLE)|A_BOLD);
        for (int i=0;i<n;i++){ 
            if(i==sel){
                attron(COLOR_PAIR(COLOR_SELECTED)|A_BOLD); 
                mvprintw(3+i,4,"> %d) %s", i+1, items[i]); 
                attroff(COLOR_PAIR(COLOR_SELECTED)|A_BOLD);
            } else {
                mvprintw(3+i,4,"  %d) %s", i+1, items[i]); 
            }
        }
        mvprintw(11,2,"SETINHA navega | ENTER seleciona | Q sai"); 
        refresh(); 
        ch=getch();
        
        if (ch=='q'||ch=='Q') {
            break;
        }
        if(ch==KEY_UP) {
            sel=(sel-1+n)%n;
        } else if(ch==KEY_DOWN) {
            sel=(sel+1)%n;
        } else if(ch=='\n') {
            if (sel==n-1) {
                break;
            }
            clear();
            if (sel==0) { 
                echo(); 
                curs_set(1); 
                char name[64]; 
                mvprintw(2,2,"Nome: "); 
                getstr(name); 
                noecho(); 
                curs_set(0); 
                if (strlen(name) > 0) { 
                    end_ncurses(); 
                    int r=cgroup_create(name); 
                    printf("create(%s) => %d\nPressione ENTER...", name,r); 
                    getchar(); 
                    init_ncurses(); 
                } else { 
                    mvprintw(4,2,"Nome inválido!"); 
                } 
            }
            else if (sel==1) { 
                echo(); 
                curs_set(1); 
                char path[128]; 
                mvprintw(2,2,"Path completo (/sys/fs/cgroup/<grupo>): "); 
                getstr(path); 
                noecho(); 
                curs_set(0); 
                if (strlen(path) > 0) { 
                    end_ncurses(); 
                    int r=cgroup_read_metrics(path); 
                    printf("read(%s) => %d\nPressione ENTER...", path,r); 
                    getchar(); 
                    init_ncurses(); 
                } else { 
                    mvprintw(4,2,"Path inválido!"); 
                } 
            }
            else if (sel==2) { 
                echo(); 
                curs_set(1); 
                char path[128], pidbuf[16]; 
                mvprintw(2,2,"Path: "); 
                getstr(path); 
                mvprintw(3,2,"PID: "); 
                getstr(pidbuf); 
                noecho(); 
                curs_set(0); 
                pid_t p=(pid_t)atoi(pidbuf); 
                if (strlen(path) > 0 && p > 0) { 
                    end_ncurses(); 
                    int r=cgroup_move_pid(path,p); 
                    printf("move(%s,%d) => %d\nPressione ENTER...", path,(int)p,r); 
                    getchar(); 
                    init_ncurses(); 
                } else { 
                    mvprintw(4,2,"Path/PID inválido!"); 
                } 
            }
            else if (sel==3) { 
                echo(); 
                curs_set(1); 
                char name[64], quota[32], period[32]; 
                mvprintw(2,2,"Nome: "); 
                getstr(name); 
                mvprintw(3,2,"Quota: "); 
                getstr(quota); 
                mvprintw(4,2,"Period: "); 
                getstr(period); 
                noecho(); 
                curs_set(0); 
                long q=atol(quota), per=atol(period); 
                if (strlen(name) > 0 && q > 0 && per > 0) { 
                    end_ncurses(); 
                    int r=cgroup_set_cpu_limit_quota(name,q,per); 
                    printf("set-cpu(%s,%ld,%ld) => %d\nPressione ENTER...", name,q,per,r); 
                    getchar(); 
                    init_ncurses(); 
                } else { 
                    mvprintw(4,2,"Valores inválidos!"); 
                } 
            }
            else if (sel==4) { 
                echo(); 
                curs_set(1); 
                char name[64], mem[32]; 
                mvprintw(2,2,"Nome: "); 
                getstr(name); 
                mvprintw(3,2,"Mem max bytes: "); 
                getstr(mem); 
                noecho(); 
                curs_set(0); 
                unsigned long m=strtoul(mem,NULL,10); 
                if (strlen(name) > 0 && m > 0) { 
                    end_ncurses(); 
                    int r=cgroup_set_memory_max(name,m); 
                    printf("set-mem(%s,%lu) => %d\nPressione ENTER...", name,m,r); 
                    getchar(); 
                    init_ncurses(); 
                } else { 
                    mvprintw(4,2,"Valores inválidos!"); 
                } 
            }
            mvprintw(8,2,"Pressione qualquer tecla para continuar..."); refresh(); getch();
        } else if (ch>='1'&&ch<='6') sel=ch-'1';
    }
    end_ncurses();
}

/* Menu de experimentos */
void show_experiments_menu() {
    init_ncurses();
    clear();
    attron(COLOR_PAIR(COLOR_TITLE) | A_BOLD);
    mvprintw(2, 2, "[!] Experimentos (1-5)");
    attroff(COLOR_PAIR(COLOR_TITLE) | A_BOLD);
    
    mvprintw(5, 2, "1) Overhead de Monitoramento");
    mvprintw(6, 2, "2) Isolamento via Namespaces");
    mvprintw(7, 2, "3) CPU Throttling");
    mvprintw(8, 2, "4) Limite de Memória");
    mvprintw(9, 2, "5) Limite de I/O");
    
    mvprintw(11, 2, "Digite o número [1-5]: ");
    refresh();
    
    echo();
    curs_set(1);
    int exp_num = getch() - '0';
    curs_set(0);
    noecho();
    
    if (exp_num >= 1 && exp_num <= 5) {
        clear();
        mvprintw(2, 2, "[!] Executando Experimento %d", exp_num);
        refresh();
        
        FILE *fp = fopen("output/experiment_log.txt", "a");
        if (!fp) { mkdir("output", 0755); fp = fopen("output/experiment_log.txt", "a"); }
        
        switch (exp_num) {
            case 1: /* Overhead */
                mvprintw(4, 2, "Medindo overhead de monitoramento...");
                refresh();
                end_ncurses();
                mkdir("output/experiments", 0755);
                OverheadResult ovh_res = {0};
                experiment_overhead(&ovh_res, "output/experiments/overhead.csv");
                init_ncurses();
                mvprintw(9, 2, "Resultado: Overhead %.2f%%", ovh_res.overhead_percent);
                mvprintw(10, 2, "Salvo em: output/experiments/overhead.csv");
                if (fp) fprintf(fp, "exp1,overhead,%.2f\n", ovh_res.overhead_percent);
                break;
                
            case 2: /* Namespaces */
                mvprintw(4, 2, "Testando isolamento de namespaces...");
                refresh();
                end_ncurses();
                mkdir("output/experiments", 0755);
                NamespaceResult ns_res = {0};
                experiment_namespace_isolation(0, &ns_res, "output/experiments/namespace.csv");
                init_ncurses();
                mvprintw(10, 2, "Resultado: %s", ns_res.isolation_verified ? "Isolado" : "Falhou");
                mvprintw(11, 2, "Salvo em: output/experiments/namespace.csv");
                if (fp) fprintf(fp, "exp2,namespace,%s\n", ns_res.test_details);
                break;
                
            case 3: /* CPU Throttling */
                mvprintw(4, 2, "Testando CPU Throttling via Cgroups...");
                refresh();
                end_ncurses();
                mkdir("output/experiments", 0755);
                CPUThrottleResult cpu_res = {0};
                experiment_cpu_throttling(50, 30, &cpu_res, "output/experiments/cpu_throttling.csv");
                init_ncurses();
                mvprintw(10, 2, "Resultado: Throttle %.1f%%", cpu_res.throttle_percent);
                mvprintw(11, 2, "Salvo em: output/experiments/cpu_throttling.csv");
                if (fp) fprintf(fp, "exp3,cpu_throttling,%.1f\n", cpu_res.throttle_percent);
                break;
                
            case 4: /* Memory */
                mvprintw(4, 2, "Testando limite de memória...");
                refresh();
                end_ncurses();
                mkdir("output/experiments", 0755);
                MemoryLimitResult mem_res = {0};
                experiment_memory_limit(256, &mem_res, "output/experiments/memory_limit.csv");
                init_ncurses();
                mvprintw(10, 2, "Resultado: %s", mem_res.oom_occurred ? "OOM killer" : "OK");
                mvprintw(11, 2, "Salvo em: output/experiments/memory_limit.csv");
                if (fp) fprintf(fp, "exp4,memory_limit,%s\n", mem_res.oom_occurred ? "oom" : "ok");
                break;
                
            case 5: /* I/O */
                mvprintw(4, 2, "Testando limite de I/O...");
                refresh();
                end_ncurses();
                mkdir("output/experiments", 0755);
                IOLimitResult io_res = {0};
                experiment_io_limit(50, 30, &io_res, "output/experiments/io_limit.csv");
                init_ncurses();
                mvprintw(10, 2, "Resultado: Slowdown %.1f%%", io_res.slowdown_percent);
                mvprintw(11, 2, "Salvo em: output/experiments/io_limit.csv");
                if (fp) fprintf(fp, "exp5,io_limit,%.1f\n", io_res.slowdown_percent);
                break;
        }
        
        if (fp) {
            fclose(fp);
        }
        
        mvprintw(13, 2, "[OK] Experimento concluído!");
        mvprintw(14, 2, "Resultados salvos em output/");
    } else {
        mvprintw(13, 2, "Experimento inválido!");
    }
    
    mvprintw(18, 2, "Pressione qualquer tecla para voltar...");
    refresh();
    getch();
    end_ncurses();
}

/**
 * Menu principal interativo
 */
int show_main_menu(void) {
    char items[5][30] = {
        "Resource Monitor",
        "Namespace Analyzer",
        "Cgroup Manager",
        "Experimentos",
        "Sair"
    };
    
    int n_items = 5;
    int selected = 0;
    int key;
    
    init_ncurses();
    
    while (1) {
        clear();
        int max_y, max_x;
        getmaxyx(stdscr, max_y, max_x);
        (void)max_x; /* Silenciar warning */
        
        /* Cabeçalho */
        attron(COLOR_PAIR(COLOR_TITLE) | A_BOLD);
        mvprintw(1, 2, "=== Resource Monitor - Menu Principal ===");
        attroff(COLOR_PAIR(COLOR_TITLE) | A_BOLD);
        
        /* Menu items */
        for (int i = 0; i < n_items; i++) {
            int y = 4 + i;
            
            if (i == selected) {
                attron(COLOR_PAIR(COLOR_SELECTED) | A_BOLD);
                mvprintw(y, 3, "> %d) %s", i + 1, items[i]);
                attroff(COLOR_PAIR(COLOR_SELECTED) | A_BOLD);
            } else {
                attron(COLOR_PAIR(COLOR_MENU));
                mvprintw(y, 3, "  %d) %s", i + 1, items[i]);
                attroff(COLOR_PAIR(COLOR_MENU));
            }
        }
        
        /* Rodapé */
        attron(COLOR_PAIR(COLOR_INFO));
        mvprintw(max_y - 2, 2, "Setinhas=Navegar | Enter=Selecionar | 1-5=Escolher | Q=Sair");
        attroff(COLOR_PAIR(COLOR_INFO));
        
        refresh();
        
        key = getch();
        
        switch (key) {
            case 'q':
            case 'Q':
                end_ncurses();
                return 5;
            case KEY_UP:
                selected = (selected - 1 + n_items) % n_items;
                break;
            case KEY_DOWN:
                selected = (selected + 1) % n_items;
                break;
            case '\n':
                end_ncurses();
                return selected + 1;
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
                end_ncurses();
                return key - '0';
        }
    }
}

/**
 * Main - Loop principal
 */
int main(int argc, char *argv[]) {
    if (argc > 1 && (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0)) {
        printf("Resource Monitor TUI\n\n");
        printf("Usage: %s [menu|--help]\n\n", argv[0]);
        printf("Commands:\n");
        printf("  menu      - Menu interativo (padrão)\n");
        printf("  --help    - Esta mensagem\n");
        return 0;
    }
    
    while (1) {
        int choice = show_main_menu();
        
        switch (choice) {
            case 1:
                show_monitor_menu();
                break;
            case 2:
                show_namespace_menu();
                break;
            case 3:
                show_cgroup_menu();
                break;
            case 4:
                show_experiments_menu();
                break;
            case 5:
                printf("Até logo!\n");
                return 0;
            default:
                return 1;
        }
    }
    
    return 0;
}
