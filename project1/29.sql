SELECT count(type)
FROM CatchedPokemon, Pokemon AS p
WHERE pid=p.id
GROUP BY type
ORDER BY type;
