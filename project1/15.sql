SELECT owner_id,count(pid) as Total
FROM CatchedPokemon GROUP BY owner_id ORDER BY owner_id limit 1;
