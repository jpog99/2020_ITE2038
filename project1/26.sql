SELECT name
FROM CatchedPokemon JOIN Pokemon AS p
WHERE pid=p.id AND nickname LIKE '% %'
ORDER BY name DESC;