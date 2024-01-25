/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: macampos <macampos@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/22 11:35:09 by macampos          #+#    #+#             */
/*   Updated: 2024/01/25 12:23:48 by macampos         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft/libft.h"
# include <stdlib.h>
# include <unistd.h>
# include <string.h>
# include <stdio.h>
# include <sys/wait.h>
# include <fcntl.h>

char	*get_paths(char *argv, char **envp)
{
	int		i;
	char	**paths;
	char	*part_path;
	char	*path;
	char	*cmd;
	

	cmd = *ft_split(argv, ' ');
	i = 0;
	if (!envp)
		return(NULL);
	while (ft_strnstr(envp[i], "PATH=", 5) == 0)
		i++;
	paths = ft_split(envp[i], ':');
	i = 0;
	while (paths[i])
	{
		part_path = ft_strjoin(paths[i], "/");
		path = ft_strjoin(part_path, cmd);
		free(part_path);
		if (access(path, F_OK) == 0)
			return (path);
		free(path);
		i++;
	}
	return (NULL);
}

void	child_process(char **argv, char **envp, int *fd)
{
	int		file;
	char	*PATH;

	close(fd[0]);
	file = open(argv[1], O_RDONLY, 0777);
	PATH = get_paths(argv[0], envp);
	if (file == -1)
		ft_printf("%s\n", "Error opening a file1");
	dup2(fd[1], STDOUT_FILENO);
	dup2(file, STDIN_FILENO);
	execve(PATH, &argv[2], envp);
	close(fd[1]);
}

void	parent_process(char **argv, char **envp, int *fd)
{
	char	*PATH;
	int		file;

	close(fd[1]);
	file = (open(argv[4], O_WRONLY | O_CREAT | O_TRUNC, 0777));
	PATH = get_paths(argv[0], envp);
	if (file == -1)
		ft_printf("%s\n", "Error opening a file2");
	dup2(fd[0], STDOUT_FILENO);
	dup2(file, STDIN_FILENO);
	execve(PATH, &argv[3], envp);
	close(fd[0]);
}

int main(int argc, char **argv, char **envp)
{
	int		id;
	int		fd[2];

	if (argc == 5)
	{
		if (pipe(fd) == -1)
		{
			ft_printf("%s\n", "Error opening the pipe");
			return (0);
		}
		id = fork();
		if (id == -1)
			return (0);
		if (id == 0)
			child_process(argv, envp, fd);
		waitpid(id, NULL, 0);
		parent_process(argv, envp, fd);
	}
	return (0);
}
