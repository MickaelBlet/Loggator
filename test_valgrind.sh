# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    test_valgrind.sh                                   :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: mblet <mblet@student.42.fr>                +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2018/04/30 18:14:39 by mblet             #+#    #+#              #
#    Updated: 2018/08/27 00:20:43 by mblet            ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

#!/bin/sh
make debug && valgrind --leak-check=full --show-leak-kinds=all ./bin/debug/Loggator && make release && valgrind --leak-check=full --show-leak-kinds=all ./bin/release/Loggator
