# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    test_valgrind.sh                                   :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: mblet <mblet@student.42.fr>                +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2018/04/30 18:14:39 by mblet             #+#    #+#              #
#    Updated: 2018/04/30 18:15:32 by mblet            ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

make debug && valgrind --leak-check=full ./bin/debug/Loggator && make release && ./bin/release/Loggator && echo "\ntest1.log" && cat test1.log && echo "\ntest2.log" && cat test2.log
