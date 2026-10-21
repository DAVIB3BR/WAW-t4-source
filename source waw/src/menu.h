/* menu.h - menu de opcoes de video do t4port (UI nativa, software).
 *
 * `t4port menu`          abre a janela de opcoes (SDL2, sem OpenGL):
 *                        - API grafica (apenas as APIs detectadas e com
 *                          backend no porte sao selecionaveis)
 *                        - resolucao, tela cheia e vsync
 *                        - tabela do sistema: o que foi detectado em cada
 *                          API (loader/ICD) e se o porte a suporta
 * `t4port menu --list`   imprime a deteccao e sai (util para CI/sem display)
 *
 * As escolhas ficam no t4port.cfg (chaves r_api, r_mode, r_fullscreen,
 * r_vsync) - o mesmo arquivo usado pelos demais comandos do porte.
 */
#ifndef T4_MENU_H
#define T4_MENU_H

int t4_menu_cmd(int argc, char **argv);

#endif /* T4_MENU_H */
