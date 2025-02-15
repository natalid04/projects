#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <readline/readline.h>
#include <readline/history.h>
#define MAX_HISTORY 1024
// Built-in command functions
void shell_cd(char **args);
void shell_exit(char **args);
void shell_help(char **args);
void shell_history(char **args);
void shell_pwd(char **args);
void shell_echo(char **args);
void shell_clear(char **args);
void shell_setenv(char **args);
void shell_unsetenv(char **args);
void shell_chprompt(char **args);
// Built-in commands list
char *builtin_str[] = {
    "cd",
    "exit",
    "help",
    "history",
    "pwd",
    "echo",
    "clear",
    "setenv",
    "unsetenv",
    "chprompt"
};
// Corresponding functions
void (*builtin_func[]) (char **) = {
    &shell_cd,
    &shell_exit,
    &shell_help,
    &shell_history,
    &shell_pwd,
    &shell_echo,
    &shell_clear,
    &shell_setenv,
    &shell_unsetenv,
    &shell_chprompt
};
int num_builtins() {
    return sizeof(builtin_str) / sizeof(char *);
}
void shell_cd(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "shell: expected argument to \"cd\"\n");
    } else {
        if (chdir(args[1]) != 0) {
            perror("shell");
        }
    }
}
void shell_exit(char **args) {
    exit(0);
}
void shell_help(char **args) {
    int i;
    printf("Built-in commands:\n");
    for (i = 0; i < num_builtins(); i++) {
        printf("  %s\n", builtin_str[i]);
    }
    printf("Use the man command for information on other programs.\n");
}
void shell_history(char **args) {
    HIST_ENTRY **hist_list;
    hist_list = history_list();
    if (hist_list) {
        for (int i = 0; hist_list[i]; i++) {
            printf("%d: %s\n", i + history_base, hist_list[i]->line);
        }
    }
}
void shell_pwd(char **args) {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s\n", cwd);
    } else {
        perror("shell");
    }
}
void shell_echo(char **args) {
    for (int i = 1; args[i]; i++) {
        printf("%s ", args[i]);
    }
    printf("\n");
}
void shell_clear(char **args) {
    printf("\033[H\033[J");
}
void shell_setenv(char **args) {
    if (args[1] == NULL || args[2] == NULL) {
        fprintf(stderr, "shell: expected argument to \"setenv\"\n");
    } else {
        if (setenv(args[1], args[2], 1) != 0) {
            perror("shell");
        }
    }
}
void shell_unsetenv(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "shell: expected argument to \"unsetenv\"\n");
    } else {
        if (unsetenv(args[1]) != 0) {
            perror("shell");
        }
    }
}
void shell_chprompt(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "shell: expected argument to \"chprompt\"\n");
    } else {
        rl_set_prompt(args[1]);
    }
}
// Signal handler
void sigint_handler(int sig) {
    printf("\n");
    rl_on_new_line();
    rl_replace_line("", 0);
    rl_redisplay();
}
// Execute command
int shell_execute(char **args) {
    int i;
    if (args[0] == NULL) {
        // An empty command was entered.
        return 1;
    }
    for (i = 0; i < num_builtins(); i++) {
        if (strcmp(args[0], builtin_str[i]) == 0) {
            (*builtin_func[i])(args);
            return 1;
        }
    }
    return shell_launch(args);
}
// Launch program
int shell_launch(char **args) {
    pid_t pid, wpid;
    int status;
    pid = fork();
    if (pid == 0) {
        // Child process
        if (execvp(args[0], args) == -1) {
            perror("shell");
        }
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        // Error forking
        perror("shell");
    } else {
        // Parent process
        do {
            wpid = waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
    return 1;
}
// Read and parse input
char *shell_read_line(void) {
    char *line = readline("> ");
    if (line && *line) {
        add_history(line);
    }
    return line;
}
char **shell_split_line(char *line) {
    int bufsize = 64, position = 0;
    char **tokens = malloc(bufsize * sizeof(char *));
    char *token;
    if (!tokens) {
        fprintf(stderr, "shell: allocation error\n");
        exit(EXIT_FAILURE);
    }
    token = strtok(line, " \t\r\n\a");
    while (token != NULL) {
        tokens[position] = token;
        position++;
        if (position >= bufsize) {
            bufsize += 64;
            tokens = realloc(tokens, bufsize * sizeof(char *));
            if (!tokens) {
                fprintf(stderr, "shell: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }
        token = strtok(NULL, " \t\r\n\a");
    }
    tokens[position] = NULL;
    return tokens;
}
// Main loop
void shell_loop(void) {
    char *line;
    char **args;
    int status;
    signal(SIGINT, sigint_handler);
    do {
        line = shell_read_line();
        args = shell_split_line(line);
        status = shell_execute(args);
        free(line);
        free(args);
    } while (status);
}
int main(int argc, char **argv) {
    rl_bind_key('\t', rl_complete);
    // Load config files, if any.
    // Run command loop.
    shell_loop();
    // Perform any shutdown/cleanup.
    return EXIT_SUCCESS;
}
