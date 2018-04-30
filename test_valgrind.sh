# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    test_valgrind.sh                                   :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: mblet <mblet@student.42.fr>                +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2018/04/30 18:14:39 by mblet             #+#    #+#              #
#    Updated: 2018/04/30 21:52:00 by mblet            ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

#!/bin/sh
make debug && valgrind --leak-check=full ./bin/debug/Loggator && make release && ./bin/release/Loggator
